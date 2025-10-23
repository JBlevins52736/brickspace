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


	if (bIsRunning)
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
{if (!pawn) return;

if (pawn->HasAuthority())
{
	bIsRunning = true;
	/*UE_LOG(LogTemp, Warning, TEXT("Server initiated StartTimer. Running: %d"), bIsRunning);*/
}
else
{
	pawn->Server_StartStopTimer(this, true);
}
	pawn->ActivateParticleSystem(true); pawn->ActivateParticleSystem(true);
}

void UTimeManager::StopTimer(ABrickSpacePawn* pawn)
{
	if (!pawn) return;

	if (pawn->HasAuthority())
	{
		bIsRunning = false;
		/*UE_LOG(LogTemp, Warning, TEXT("Server initiated StopTimer. Running: %d"), bIsRunning);*/
	}
	else
	{
		pawn->Server_StartStopTimer(this, false);
	}
	pawn->ActivateParticleSystem(false);
}


void UTimeManager::ResetTimer(ABrickSpacePawn* pawn)
{
	if (!pawn) return;

	if (pawn->HasAuthority())
	{
		Multicast_ResetTimer();
		//UE_LOG(LogTemp, Warning, TEXT("Server initiated ResetTimer."));
	}
	else
	{
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

void UTimeManager::OnRep_Running()
{
	
	/*UE_LOG(LogTemp, Warning, TEXT("OnRep_Running called. bIsRunning: %d"), bIsRunning);*/

	
	if (!bIsRunning)
	{
		
		UpdateTextRenderer();

		if (!GetOwner()->HasAuthority())
		{
			/*UE_LOG(LogTemp, Warning, TEXT("Client sending final time to server: %.2f"), ElapsedTime);*/
			Server_SyncStoppedTime(ElapsedTime);
		}
	}
}


void UTimeManager::Multicast_ResetTimer_Implementation()
{
	// This runs on SERVER and ALL CLIENTS
	ElapsedTime = 0.0f;

	// Only server resets this
	if (GetOwner()->HasAuthority())
	{
		ServerStoppedTime = 0.0f;
	}

	UpdateTextRenderer();

	//UE_LOG(LogTemp, Warning, TEXT("Machine received Reset Timer command. Local Time Reset."));
}

void UTimeManager::Server_SyncStoppedTime_Implementation(float FinalTime)
{
	
	ServerStoppedTime = FinalTime;
	/*UE_LOG(LogTemp, Warning, TEXT("Server received final time from client: %.2f"), ServerStoppedTime);*/
}

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