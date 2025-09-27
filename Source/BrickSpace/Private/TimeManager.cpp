#include "TimeManager.h"
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
		// Optional: Log or update text
		UE_LOG(LogTemp, Log, TEXT("Elapsed Time: %.2f"), ElapsedTime);
	}
}

void UTimeManager::StartTimer()
{
	ElapsedTime = 0.0f;
	bIsRunning = true;
	UE_LOG(LogTemp, Log, TEXT("Timer started."));
}

void UTimeManager::StopTimer()
{
	bIsRunning = false;
	UE_LOG(LogTemp, Warning, TEXT("Timer stopped. Final time: %.2f seconds"), ElapsedTime);
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
		// Format to 2 decimal places
		FString TimeString = FString::Printf(TEXT("%.2f"), ElapsedTime);
		TimerTextRenderer->SetText(FText::FromString(TimeString));
	}
}