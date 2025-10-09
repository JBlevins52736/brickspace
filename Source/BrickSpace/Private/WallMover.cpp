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

    CurrentPercentage = Percentage;


    if (Percentage < 0.0f)
    {
        
        TargetRelativeLocation = LoweredRelativeLocation;
    }
    else if (Percentage > 0.0f)
    {
        
        TargetRelativeLocation = InitialRelativeLocation;
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
}

void UWallMover::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    float PercentageAbs = FMath::Abs(CurrentPercentage);

    const float BaseSpeedFactor = 3.0f; 

    FVector NewLocation = UKismetMathLibrary::VInterpTo(
        GetRelativeLocation(),
        TargetRelativeLocation,
        DeltaTime,
        (BaseSpeedFactor * PercentageAbs)
    );

    SetRelativeLocation(NewLocation);
}
