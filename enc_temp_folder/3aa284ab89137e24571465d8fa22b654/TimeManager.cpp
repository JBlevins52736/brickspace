#include "TimeManager.h"
#include "BrickSpacePawn.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Actor.h"
#include "Components/TextRenderComponent.h" 

UTimeManager::UTimeManager()
{
	PrimaryComponentTick.bCanEverTick = true;

	ElapsedTime = 0.0f;
	bIsRunning = false;
	TimerTextRenderer = nullptr;
	SetIsReplicatedByDefault(true);
}

void UTimeManager::BeginPlay()
{
	Super::BeginPlay();
}

void UTimeManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Every instance (client and server) runs this if the authoritative bIsRunning is true
	if (bIsRunning)
	{
		ElapsedTime += DeltaTime;
		UpdateTextRenderer();
	}
}

void UTimeManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// Only the running state is replicated; time is calculated locally.
	DOREPLIFETIME(UTimeManager, bIsRunning);
}

// --- Public Functions (Called by Button) ---

void UTimeManager::StartTimer(ABrickSpacePawn* pawn)
{
	if (!pawn) return;

	if (pawn->HasAuthority())
	{
		// 1. Server sets authoritative state
		bIsRunning = true;
		UE_LOG(LogTemp, Warning, TEXT("Server initiated StartTimer. Running: %d"), bIsRunning);

		// 2. Server commands all clients (including itself) to ensure local start
		Client_StartTimer();
	}
	else
	{
		// 1. Client asks server to start
		pawn->Server_StartStopTimer(this, true);
	}
}

void UTimeManager::StopTimer(ABrickSpacePawn* pawn)
{
	if (!pawn) return;

	if (pawn->HasAuthority())
	{
		// 1. Server sets authoritative state
		bIsRunning = false;
		UE_LOG(LogTemp, Warning, TEXT("Server initiated StopTimer. Running: %d"), bIsRunning);

		// 2. Server commands all clients (including itself) to stop and report
		Client_StopTimer();
	}
	else
	{
		// 1. Client asks server to stop
		pawn->Server_StartStopTimer(this, false);
	}
}


void UTimeManager::ResetTimer(ABrickSpacePawn* pawn)
{
	if (!pawn) return;

	if (pawn->HasAuthority())
	{
		if (!bIsRunning)
		{
			// 1. Server resets its local copy and command
			ElapsedTime = 0.0f;
			ServerStoppedTime = 0.0f;
			UE_LOG(LogTemp, Warning, TEXT("Server initiated ResetTimer."));

			// 2. Server commands all clients (including itself) to reset local time
			Client_ResetTimer();
		}
	}
	else
	{
		// 1. Client asks server to reset
		pawn->Server_ResetTimer(this);
	}
}

float UTimeManager::GetElapsedTime() const
{
	return ElapsedTime;
}

void UTimeManager::SetTextRenderer(UTextRenderComponent* InTextRenderer)
{
	TimerTextRenderer = InTextRenderer;
	UpdateTextRenderer();
}

// --- Replication and Client RPC Implementations ---

void UTimeManager::OnRep_Running()
{
	// Triggered when the authoritative bIsRunning state changes.
	UE_LOG(LogTemp, Warning, TEXT("OnRep_Running called. bIsRunning: %d"), bIsRunning);

	// If the timer stops, ensure the display is updated to the final local time.
	if (!bIsRunning)
	{
		UpdateTextRenderer();
	}
}

void UTimeManager::Client_StartTimer_Implementation()
{
	// Command received to start. TickComponent will handle time accumulation.
	UE_LOG(LogTemp, Warning, TEXT("Client received Start Timer command."));
}

void UTimeManager::Client_StopTimer_Implementation()
{
	// Command received to stop.

	// 1. Only clients report their final time to the server.
	if (!GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Client sending final time to server: %.2f"), ElapsedTime);
		// 2. Client calls the Server RPC to synchronize its final time.
		Server_SyncStoppedTime(ElapsedTime);
	}

	// The replicated bIsRunning flag will be set to false, stopping the Tick.
}

void UTimeManager::Client_ResetTimer_Implementation()
{
	// Reset the client's local time immediately
	ElapsedTime = 0.0f;
	UpdateTextRenderer();
	UE_LOG(LogTemp, Warning, TEXT("Client received Reset Timer command. Local Time Reset."));
}

void UTimeManager::Server_SyncStoppedTime_Implementation(float FinalTime)
{
	// This RPC is executed on the server, saving the client's precise time.
	ServerStoppedTime = FinalTime;
	UE_LOG(LogTemp, Warning, TEXT("Server received final time from client: %.2f"), ServerStoppedTime);
}

// --- Text Formatting ---

void UTimeManager::UpdateTextRenderer()
{

	if (TimerTextRenderer)
	{
		int TotalMilliseconds = static_cast<int32>(ElapsedTime * 1000.0f);

		int Minutes = TotalMilliseconds / 60000;
		int Seconds = (TotalMilliseconds % 60000) / 1000;
		int Centiseconds = (TotalMilliseconds % 1000) / 10;

		FString TimeString = FString::Printf(TEXT("%02d:%02d:%02d"), Minutes, Seconds, Centiseconds);

		TimerTextRenderer->SetText(FText::FromString(TimeString));
	}
}