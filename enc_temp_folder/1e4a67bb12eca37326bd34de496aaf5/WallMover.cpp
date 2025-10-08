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

    CurrentLeverAngle = LeverAngle;

    if (!bIsInitialized) return;

    // The LeverAngle you receive here is the RAW CLAMPED ANGLE (e.g., -90.0 to 45.0) 
    // from the broadcast, NOT the Normalized 0.0 to 1.0 value.

    // 1. Normalize the full lever range (using your angle thresholds as the full range)
    const float MinRange = LoweredThreshold; // e.g., -80.0f (Lowest angle)
    const float MaxRange = RaisedThreshold;

    // Clamp the incoming angle to ensure it stays within the expected range
    const float ClampedLeverAngle = FMath::Clamp(LeverAngle, MinRange, MaxRange);

    // InverseLerp: Calculates the Alpha (0.0 to 1.0) based on the clamped angle
    // Alpha will be 0.0 at MinRange (Down position) and 1.0 at MaxRange (Up position)
    float Alpha = UKismetMathLibrary::NormalizeToRange(ClampedLeverAngle, MinRange, MaxRange);

    // 2. Map this Alpha to the Z-axis movement range.
    FVector RaisedLocation = InitialRelativeLocation; // This is the 'original' UP position.
    FVector LoweredLocation = InitialRelativeLocation + FVector(0.0f, 0.0f, LoweredZOffset); // This is the DOWN position.

    // FIX: Use Lerp to map Alpha to the correct Z range.
    // Alpha = 0.0 (Lever Down) -> Lerps to LoweredLocation.Z
    // Alpha = 1.0 (Lever Up) -> Lerps to RaisedLocation.Z
    TargetRelativeLocation.Z = FMath::Lerp(LoweredLocation.Z, RaisedLocation.Z, Alpha);

    // Ensure X and Y are set to the initial location to prevent lateral movement
    TargetRelativeLocation.X = InitialRelativeLocation.X;
    TargetRelativeLocation.Y = InitialRelativeLocation.Y;

    // Log for debugging
    UE_LOG(LogTemp, Log, TEXT("Lever Angle: %.2f | Alpha: %.2f | Target Z: %.2f"), LeverAngle, Alpha, TargetRelativeLocation.Z);
}


//void UWallMover::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//    if (!bIsInitialized) return;
//    // ... Tick logic remains here, using DeltaTime correctly ...
//
//    FVector CurrentLocation = GetRelativeLocation();
//
//    if (CurrentLocation.Equals(TargetRelativeLocation, 0.01f))
//    {
//        return;
//    }
//
//    // This section correctly uses DeltaTime (which is passed to TickComponent)
//    FVector NewLocation = UKismetMathLibrary::VInterpTo(
//        CurrentLocation,
//        TargetRelativeLocation,
//        DeltaTime, // <-- DeltaTime is valid here
//        MovementInterpSpeed * 5.0f // Increased speed factor
//    );
//
//    SetRelativeLocation(NewLocation);
//}
void UWallMover::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!bIsInitialized) return;

    FVector CurrentLocation = GetRelativeLocation();

    if (CurrentLocation.Equals(TargetRelativeLocation, 0.01f))
    {
        return;
    }

    // 1. Calculate the magnitude of the lever angle away from the center (0.0).
    // The center position (0.0) is typically where the wall is halfway between raised and lowered.
    const float AngleFromCenter = FMath::Abs(CurrentLeverAngle);

    // 2. Determine the dynamic speed factor based on the angle.
    // We normalize the angle (0.0 to MaxAbsAngle) to a factor (1.0 to MaxFactor).

    // Find the maximum magnitude of the angle range (e.g., if range is -80 to 30, MaxAbsAngle is 80)
    const float MaxAbsAngle = FMath::Max(FMath::Abs(LoweredThreshold), FMath::Abs(RaisedThreshold));

    // Normalize AngleFromCenter from 0 to MaxAbsAngle to a factor from 0.0 to 1.0
    float AngleFactor = FMath::GetMappedRangeValueClamped(
        FVector2D(0.0f, MaxAbsAngle),
        FVector2D(0.0f, 1.0f),
        AngleFromCenter
    );

    // Apply a multiplier to ensure a noticeable speed difference.
    // MaxFactor: When AngleFactor is 1.0, the wall moves at 5x the base speed (adjust 5.0f for feel).
    const float MaxSpeedMultiplier = 5.0f;

    // Ensure the speed is at least the base speed (by adding 1.0 to the factor before multiplying)
    const float DynamicSpeedMultiplier = 1.0f + (AngleFactor * MaxSpeedMultiplier);

    // 3. Calculate the final Interp Speed.
    const float FinalInterpSpeed = MovementInterpSpeed * DynamicSpeedMultiplier;

    // Log for debugging
    UE_LOG(LogTemp, Verbose, TEXT("Angle: %.2f | AngleFactor: %.2f | Final Speed: %.2f"), CurrentLeverAngle, AngleFactor, FinalInterpSpeed);


    // 4. Interpolate to the target using the dynamic speed.
    FVector NewLocation = UKismetMathLibrary::VInterpTo(
        CurrentLocation,
        TargetRelativeLocation,
        DeltaTime,
        FinalInterpSpeed // Use the dynamically calculated speed
    );

    SetRelativeLocation(NewLocation);
}