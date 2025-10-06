#include "WallMover.h"
#include "WallLever.h" 
#include "Kismet/KismetMathLibrary.h" 

// Renamed constructor
UWallMover::UWallMover()
{
	
	PrimaryComponentTick.bCanEverTick = true;
}

void UWallMover::BeginPlay()
{
	Super::BeginPlay();

	 
		InitialRelativeLocation = GetRelativeLocation();
		TargetRelativeLocation = InitialRelativeLocation; 
		bIsInitialized = true;
	


}

void UWallMover::SetMovementTarget(float LeverAngle)
{
	if (!bIsInitialized) return;

	if (LeverAngle <= LoweredThreshold)
	{
		
		TargetRelativeLocation = InitialRelativeLocation + FVector(0.0f, 0.0f, LoweredZOffset);
		UE_LOG(LogTemp, Log, TEXT("Lever at %f, Setting Target DOWN to %s"), LeverAngle, *TargetRelativeLocation.ToString());
	}
	
	else if (LeverAngle >= RaisedThreshold)
	{
		
		TargetRelativeLocation = InitialRelativeLocation;
		UE_LOG(LogTemp, Log, TEXT("Lever at %f, Setting Target UP to %s"), LeverAngle, *TargetRelativeLocation.ToString());
	}
}


void UWallMover::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bIsInitialized ) return;

	FVector CurrentLocation = GetRelativeLocation();

	if (CurrentLocation.Equals(TargetRelativeLocation, 0.01f))
	{
		return;
	}


	FVector NewLocation = UKismetMathLibrary::VInterpTo(
		CurrentLocation,
		TargetRelativeLocation,
		DeltaTime,
		MovementInterpSpeed
	);

	SetRelativeLocation(NewLocation);
}