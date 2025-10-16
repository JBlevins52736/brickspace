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
    RaisedRelativeLocation = InitialRelativeLocation + FVector(0.0f, 0.0f, RaisedZOffset);

   
    TargetRelativeLocation = InitialRelativeLocation;
}

void UWallMover::SetMovementTarget(float Percentage)
{
    CurrentPercentage = Percentage;

    // ... rest of the logic (now only running on the server) ...
    if (Percentage < 0.0f)
    {
        TargetRelativeLocation = LoweredRelativeLocation;
    }
    else if (Percentage > 0.0f)
    {
        TargetRelativeLocation = RaisedRelativeLocation;
    }
    else
    {
        TargetRelativeLocation = GetRelativeLocation();
    }
}


void UWallMover::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UWallMover, TargetRelativeLocation);
    DOREPLIFETIME(UWallMover, CurrentPercentage);
}

void UWallMover::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    float PercentageAbs = FMath::Abs(CurrentPercentage);

    const float BaseSpeedFactor = 2.0f; 

    FVector NewLocation = UKismetMathLibrary::VInterpTo(
        GetRelativeLocation(),
        TargetRelativeLocation,
        DeltaTime,
        (BaseSpeedFactor * PercentageAbs)
    );

    SetRelativeLocation(NewLocation);

    if (FloorPlate)
    {
        float TargetGroundScaleY = 0.0f;

        if (CurrentPercentage == 0.0f)
        {
            bWallIsClear = false;
        }
        else
        {
            const float LocationTolerance = 0.1f;
            if (FVector::DistSquared(GetRelativeLocation(), TargetRelativeLocation) < LocationTolerance * LocationTolerance)
            {
                bWallIsClear = true;
            }
        }

        if (CurrentPercentage < 0.0f)
        {
           
            if (bWallIsClear)
            {
                TargetGroundScaleY = GroundMaxScaleY;
            }

            TargetGroundScaleY = GroundMaxScaleY;
        }

        FVector CurrentScale = FloorPlate->GetRelativeScale3D();
        float NewScaleY = FMath::FInterpTo(
            CurrentScale.Y,
            TargetGroundScaleY,
            DeltaTime,
            GroundScaleInterpSpeed
        );
        if (CurrentScale.Y >= GroundMaxScaleY && !bWallIsClear && CurrentPercentage < 0.0f)
        {
            NewScaleY = GroundMaxScaleY;
        }

        
        NewScaleY = FMath::Clamp(NewScaleY, 0.0f, GroundMaxScaleY);

        FloorPlate->SetRelativeScale3D(FVector(CurrentScale.X, NewScaleY, CurrentScale.Z));
    }
}
