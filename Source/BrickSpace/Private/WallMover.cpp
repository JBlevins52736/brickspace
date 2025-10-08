#include "WallMover.h"
#include "WallLever.h" // Assuming this is where the angle comes from
#include "Kismet/KismetMathLibrary.h" // Still useful for clamping

// Renamed constructor
UWallMover::UWallMover()
{
    // NOTE: We will disable TickComponent entirely, or only use it for final smoothing if desired.
    // For direct velocity control, we prefer to update in SetMovementTarget.
    PrimaryComponentTick.bCanEverTick = true; // Keep true for now, but its body will be empty.
}

void UWallMover::BeginPlay()
{
    Super::BeginPlay();

    InitialRelativeLocation = GetRelativeLocation();
    TargetRelativeLocation = InitialRelativeLocation;
    bIsInitialized = true;

    // Set a practical boundary for the TargetRelativeLocation's Z-axis
    TargetRelativeLocation.Z = InitialRelativeLocation.Z + LoweredZOffset;
}

void UWallMover::SetMovementTarget(float LeverAngle)
{
    if (!bIsInitialized) return;

    // -----------------------------------------------------------
    // The previous logic using DeltaTime (now removed) was here.
    // We proceed directly to using the angle to set the interpolation target (Lerp).
    // -----------------------------------------------------------

    // 1. Normalize the full lever range (using your thresholds as the full range)
    const float MinRange = LoweredThreshold; // e.g., -80.0f
    const float MaxRange = RaisedThreshold;  // e.g., 30.0f

    // Clamp the incoming angle to ensure it stays within the expected range
    const float ClampedLeverAngle = FMath::Clamp(LeverAngle, MinRange, MaxRange);

    // InverseLerp: Calculates the Alpha (0.0 to 1.0) based on the clamped angle
    // Alpha will be 0.0 at MinRange (Lowered) and 1.0 at MaxRange (Raised)
    float Alpha = UKismetMathLibrary::NormalizeToRange(ClampedLeverAngle, MinRange, MaxRange);

    // 2. Map this Alpha to the Z-axis movement range.
    FVector RaisedLocation = InitialRelativeLocation;
    FVector LoweredLocation = InitialRelativeLocation + FVector(0.0f, 0.0f, LoweredZOffset);

    // Lerp: Use the Alpha to find the target Z position between the Lowered and Raised limits.
    // NOTE: This assumes you want the wall to be LOWERED when the angle is smallest (MinRange) 
    // and RAISED when the angle is largest (MaxRange). 

    TargetRelativeLocation.Z = FMath::Lerp(LoweredLocation.Z, RaisedLocation.Z, Alpha);

    // Log for debugging
    UE_LOG(LogTemp, Log, TEXT("Lever Angle: %.2f | Alpha: %.2f | Target Z: %.2f"), LeverAngle, Alpha, TargetRelativeLocation.Z);
}


void UWallMover::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!bIsInitialized) return;
    // ... Tick logic remains here, using DeltaTime correctly ...

    FVector CurrentLocation = GetRelativeLocation();

    if (CurrentLocation.Equals(TargetRelativeLocation, 0.01f))
    {
        return;
    }

    // This section correctly uses DeltaTime (which is passed to TickComponent)
    FVector NewLocation = UKismetMathLibrary::VInterpTo(
        CurrentLocation,
        TargetRelativeLocation,
        DeltaTime, // <-- DeltaTime is valid here
        MovementInterpSpeed * 5.0f // Increased speed factor
    );

    SetRelativeLocation(NewLocation);
}