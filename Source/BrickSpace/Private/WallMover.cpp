#include "WallMover.h"
#include "WallLever.h" // Include your Lever header
#include "Kismet/KismetMathLibrary.h" // For VInterpTo

// Renamed constructor
UWallMover::UWallMover()
{
	// Ensure the Tick function runs for smoothing the movement
	PrimaryComponentTick.bCanEverTick = true;
}

void UWallMover::BeginPlay()
{
	Super::BeginPlay();

	 
		InitialRelativeLocation = GetRelativeLocation();
		TargetRelativeLocation = InitialRelativeLocation; // Start target at initial position
		bIsInitialized = true;
	


}

// --- BlueprintCallable Function: Handles the lever event and sets the target ---
void UWallMover::SetMovementTarget(float LeverAngle)
{
	if (!bIsInitialized) return;

	// If the lever is pushed down past the threshold (-80)
	if (LeverAngle <= LoweredThreshold)
	{
		// Set target to the lowered position (Initial Z + Offset)
		TargetRelativeLocation = InitialRelativeLocation + FVector(0.0f, 0.0f, LoweredZOffset);
		UE_LOG(LogTemp, Log, TEXT("Lever at %f, Setting Target DOWN to %s"), LeverAngle, *TargetRelativeLocation.ToString());
	}
	// If the lever is pushed up past the threshold (30)
	else if (LeverAngle >= RaisedThreshold)
	{
		// Set target back to the original raised position
		TargetRelativeLocation = InitialRelativeLocation;
		UE_LOG(LogTemp, Log, TEXT("Lever at %f, Setting Target UP to %s"), LeverAngle, *TargetRelativeLocation.ToString());
	}
}


// --- Tick Function: Smoothly moves the component towards the target ---
void UWallMover::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bIsInitialized ) return;

	FVector CurrentLocation = GetRelativeLocation();

	// Check if we are already at the target to stop continuous interpolation
	if (CurrentLocation.Equals(TargetRelativeLocation, 0.01f))
	{
		return;
	}

	// Interpolate (smoothly move) the current location towards the target location
	// This creates the continuous, slow motion you requested.
	FVector NewLocation = UKismetMathLibrary::VInterpTo(
		CurrentLocation,
		TargetRelativeLocation,
		DeltaTime,
		MovementInterpSpeed
	);

	// Set the new location on the wall
	SetRelativeLocation(NewLocation);
}