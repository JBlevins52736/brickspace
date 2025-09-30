#include "TimeManager.h"
#include "BrickSpacePawn.h"

#include "GameFramework/Actor.h"

// Constructor
UTimeManager::UTimeManager()
{
	PrimaryComponentTick.bCanEverTick = true;

	ElapsedTime = 0.0f;
	bIsRunning = false;
	TimerTextRenderer = nullptr;
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

void UTimeManager::StartTimer(ABrickSpacePawn* pawn)
{
	// Modify this method to take in a pawn ref
	// Make an rpc call in the server pawn
	// Decide if this is the host or a remote client
	if (pawn->HasAuthority() && pawn->IsLocallyControlled()) {
		
		bIsRunning = true;
		
	}
	else if (pawn->GetLocalRole() == ROLE_AutonomousProxy) {
		pawn->Server_StartStopTimer(this, true);
	}
	ElapsedTime = 0.0f;
	
}

void UTimeManager::StopTimer(ABrickSpacePawn* pawn)
{/*
	bIsRunning = false;*/
	if (pawn->HasAuthority() && pawn->IsLocallyControlled()) {

		bIsRunning = false;

	}
	else if (pawn->GetLocalRole() == ROLE_AutonomousProxy) {
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