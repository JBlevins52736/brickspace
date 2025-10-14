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
}
