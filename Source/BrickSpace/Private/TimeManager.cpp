#include "TimeManager.h"
#include "BrickSpacePawn.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Actor.h"

// Constructor
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

	// Only tick on server/authority
	if (bIsRunning && GetOwner() && GetOwner()->HasAuthority())
	{
		ElapsedTime += DeltaTime;
		UpdateTextRenderer();
	}
}

void UTimeManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UTimeManager, bIsRunning);
	DOREPLIFETIME(UTimeManager, ElapsedTime);
}

void UTimeManager::StartTimer(ABrickSpacePawn* pawn)
{
	// Modify this method to take in a pawn ref
	// Make an rpc call in the server pawn
	// Decide if this is the host or a remote client
	if (!pawn) return;

	bool bHasAuth = pawn->HasAuthority() /*&& pawn->IsLocallyControlled()*/;
	//bool bIsClient = pawn->GetLocalRole() == ROLE_AutonomousProxy;

	if (bHasAuth)
	{
		// TOGGLE LOGIC
		if (bIsRunning)
		{
			// Pause
			bIsRunning = false;
			UE_LOG(LogTemp, Warning, TEXT("Timer paused. ElapsedTime: %.2f"), ElapsedTime);
		}
		else
		{
			// Start or Resume (but don't reset time!)
			bIsRunning = true;
			UE_LOG(LogTemp, Warning, TEXT("Timer started/resumed. ElapsedTime: %.2f"), ElapsedTime);
		}
		// Force update on server
		UpdateTextRenderer();
	}
	else /*if (bIsClient)*/
	{
		pawn->Server_StartStopTimer(this, !bIsRunning); // Toggle based on current state

	}
	
}

void UTimeManager::StopTimer(ABrickSpacePawn* pawn)
{
	if (!pawn) return;

	bool bHasAuth = pawn->HasAuthority() /*&& pawn->IsLocallyControlled()*/;
	//bool bIsClient = pawn->GetLocalRole() == ROLE_AutonomousProxy;

	// Only allow reset when paused
	if (bHasAuth) 
	{
		if (!bIsRunning)
		{
			ElapsedTime = 0.0f;
			UpdateTextRenderer();
			UE_LOG(LogTemp, Warning, TEXT("Timer reset to 0."));
			/*pawn->Server_StartStopTimer_Implementation(this, true);*/
		}
		else
		{
			// Stop the timer
			bIsRunning = false;
			UpdateTextRenderer();
		}
	}
	else /*if (bIsClient)*/
	{
		pawn->Server_StartStopTimer(this, false);
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

void UTimeManager::OnRep_Running()
{
	UpdateTextRenderer();
	UE_LOG(LogTemp, Warning, TEXT("OnRep_Running called. bIsRunning: %d, ElapsedTime: %.2f"), bIsRunning, ElapsedTime);
}

void UTimeManager::OnRep_ElapsedTime()
{
	// Update display when elapsed time replicates
	UpdateTextRenderer();
}


void UTimeManager::UpdateTextRenderer()
{
	
	if (TimerTextRenderer)
	{
		int TotalMilliseconds = static_cast<int32>(ElapsedTime * 1000.0f);

		int Minutes = TotalMilliseconds / 60000;                        // 60,000 ms in a minute
		int Seconds = (TotalMilliseconds % 60000) / 1000;               // remainder / 1000 = seconds
		int Centiseconds = (TotalMilliseconds % 1000) / 10;             // ms / 10 = 2 digits (00–99)

		// Format: MM:SS:CC (zero-padded)
		FString TimeString = FString::Printf(TEXT("%02d:%02d:%02d"), Minutes, Seconds, Centiseconds);

		TimerTextRenderer->SetText(FText::FromString(TimeString));
	}
}


