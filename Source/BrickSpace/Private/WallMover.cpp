#include "WallMover.h"
#include "WallLever.h" 
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

UWallMover::UWallMover()
{
    PrimaryComponentTick.bCanEverTick = true;
    SetIsReplicatedByDefault(true); 
}

void UWallMover::BeginPlay()
{
    Super::BeginPlay();

    InitialRelativeLocation = GetRelativeLocation();
    LoweredRelativeLocation = InitialRelativeLocation + FVector(0.0f, 0.0f, LoweredZOffset);

   
    TargetRelativeLocation = InitialRelativeLocation;
}

void UWallMover::SetMovementTarget(float Percentage)
{
    // Store the incoming percentage (e.g., -1.0 to 1.0)
    // The previous logic for alpha based on LoweredThreshold and RaisedThreshold is now
    // handled by the external component broadcasting Percentage.
    CurrentPercentage = Percentage;

    // In this model, the TargetRelativeLocation will be the fully lowered or fully
    // raised position, and the VInterpTo speed will vary.

    if (Percentage < 0.0f)
    {
        // Negative percentage: moving towards the lowered position
        TargetRelativeLocation = LoweredRelativeLocation;
    }
    else if (Percentage > 0.0f)
    {
        // Positive percentage: moving towards the initial (raised) position
        TargetRelativeLocation = InitialRelativeLocation;
    }
    else
    {
        // Percentage is 0: target is the current location (wall stops moving)
        TargetRelativeLocation = GetRelativeLocation();
    }
}


void UWallMover::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UWallMover, TargetRelativeLocation);
}

void UWallMover::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // Get the speed multiplier based on the absolute percentage (0.0 to 1.0)
    float PercentageAbs = FMath::Abs(CurrentPercentage);

    // If the wall is at the target AND the input is 0, we can stop early.
    // If the input is not 0, we still need to run the VInterpTo to reach the target.
    // NOTE: Checking equality with floats can be unreliable. Using a small tolerance is better.
    if (FMath::IsNearlyEqual(PercentageAbs, 0.0f) && GetRelativeLocation().Equals(TargetRelativeLocation, 0.01f))
    {
        return;
    }

    // New InterpSpeed calculation: MovementInterpSpeed * PercentageAbs
    // The percentage is already normalized (0 to 1), so no need to divide it by anything else.
    // We can use a reasonable constant factor for overall movement speed.
    const float BaseSpeedFactor = 100.0f; // A constant for base movement speed (adjust as needed)

    FVector NewLocation = UKismetMathLibrary::VInterpTo(
        GetRelativeLocation(),
        TargetRelativeLocation,
        DeltaTime,
        (BaseSpeedFactor * PercentageAbs) // Speed increases directly with PercentageAbs
    );

    SetRelativeLocation(NewLocation);
}
