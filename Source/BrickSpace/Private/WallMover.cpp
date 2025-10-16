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
    GEngine->AddOnScreenDebugMessage(
        -1,                      // Key to identify/replace the message (use -1 for new)
        5.0f,                    // Duration to display (in seconds)
        FColor::Green,          // Color of the text
        FString::Printf(TEXT("Wall Z Location: %f"), NewLocation.Z));

    if (FloorPlate)
    {
        float WallZ = GetRelativeLocation().Z;
        if (WallZ <= 10) {
    // Scale FloorPlate up (to 1.2f)
    FVector CurrentScale = FloorPlate->GetRelativeScale3D();
    float NewScaleY = FMath::FInterpTo(CurrentScale.Y, 1.2f, DeltaTime, GroundScaleInterpSpeed);
    FloorPlate->SetRelativeScale3D(FVector(CurrentScale.X, NewScaleY, CurrentScale.Z));
}
else if (WallZ > 10) // Changed to 10 to cover the remaining range clearly
{
    // Scale FloorPlate down (to 0.0f)
    FVector CurrentScale = FloorPlate->GetRelativeScale3D();
    float NewScaleY = FMath::FInterpTo(CurrentScale.Y, 0.0f, DeltaTime, GroundScaleInterpSpeed);
    FloorPlate->SetRelativeScale3D(FVector(CurrentScale.X, NewScaleY, CurrentScale.Z));
}
// Note: WallZ = 10 is covered by the first 'if'

// Logic for SkyPlate (Starts here, NOT an 'else if' of the above)
if (WallZ > 60)
{
    // Scale SkyPlate up (to 1.2f)
    FVector CurrentScale = SkyPlate->GetRelativeScale3D();
    float NewScaleY = FMath::FInterpTo(CurrentScale.Y, 1.2f, DeltaTime, GroundScaleInterpSpeed);
    SkyPlate->SetRelativeScale3D(FVector(CurrentScale.X, NewScaleY, CurrentScale.Z));
}
else if (WallZ < 50) // The condition WallZ < 50 should be the opposite of WallZ > 60's active range
{
    // Scale SkyPlate down (to 0.0f)
    FVector CurrentScale = SkyPlate->GetRelativeScale3D();
    float NewScaleY = FMath::FInterpTo(CurrentScale.Y, 0.0f, DeltaTime, GroundScaleInterpSpeed);
    SkyPlate->SetRelativeScale3D(FVector(CurrentScale.X, NewScaleY, CurrentScale.Z));
}

       
    }
}
