#include "WallMover.h"
#include "WallLever.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"


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

    TargetRelativeLocation.Z = InitialRelativeLocation.Z + LoweredZOffset;
}

void UWallMover::SetMovementTarget(float LeverAngle)
{
    CurrentLeverAngle = LeverAngle;
    if (!bIsInitialized) return;

    const float MinRange = LoweredThreshold; 
    const float MaxRange = RaisedThreshold;  


    const float ClampedLeverAngle = FMath::Clamp(LeverAngle, MinRange, MaxRange);
    float Alpha = UKismetMathLibrary::NormalizeToRange(ClampedLeverAngle, MinRange, MaxRange);

    FVector RaisedLocation = InitialRelativeLocation;
    FVector LoweredLocation = InitialRelativeLocation + FVector(0.0f, 0.0f, LoweredZOffset);

    TargetRelativeLocation.Z = FMath::Lerp(LoweredLocation.Z, RaisedLocation.Z, Alpha);

    TargetRelativeLocation.X = InitialRelativeLocation.X;
    TargetRelativeLocation.Y = InitialRelativeLocation.Y;
    UE_LOG(LogTemp, Log, TEXT("SERVER: Lever Angle: %.2f | Alpha: %.2f | Target Z: %.2f"), LeverAngle, Alpha, TargetRelativeLocation.Z); 
}

void UWallMover::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UWallMover, TargetRelativeLocation);
}

void UWallMover::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!bIsInitialized) return;

    FVector CurrentLocation = GetRelativeLocation();

    if (CurrentLocation.Equals(TargetRelativeLocation, 0.01f))
    {
        return;
    }

    const float AngleFromCenter = FMath::Abs(CurrentLeverAngle);
    const float MaxAbsAngle = FMath::Max(FMath::Abs(LoweredThreshold), FMath::Abs(RaisedThreshold));

    float AngleFactor = FMath::GetMappedRangeValueClamped(
        FVector2D(0.0f, MaxAbsAngle),
        FVector2D(0.0f, 1.0f),
        AngleFromCenter
    );

    const float MaxSpeedMultiplier = 5.0f;

    const float DynamicSpeedMultiplier = 1.0f + (AngleFactor * MaxSpeedMultiplier);

    const float FinalInterpSpeed = MovementInterpSpeed * DynamicSpeedMultiplier;
    UE_LOG(LogTemp, Verbose, TEXT("Angle: %.2f | AngleFactor: %.2f | Final Speed: %.2f"), CurrentLeverAngle, AngleFactor, FinalInterpSpeed);

    FVector NewLocation = UKismetMathLibrary::VInterpTo(
        CurrentLocation,
        TargetRelativeLocation,
        DeltaTime,
        FinalInterpSpeed
    );

    SetRelativeLocation(NewLocation);
}