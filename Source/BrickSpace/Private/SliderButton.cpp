#include "SliderButton.h"
#include "BrickSpacePawn.h"

#include "Selector.h"

USliderButton::USliderButton()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = false;

}

void USliderButton::BeginPlay()
{
    Super::BeginPlay();
}

void USliderButton::Focus(USelector* cursor, bool state)
{
    Super::Focus(cursor, state);

    if (!clientComponent || !cursor) return;

    if (state)
    {
        PrimaryComponentTick.SetTickFunctionEnable(state);
        focusSelector = cursor;

        FVector CursorWorldLocation = focusSelector->Cursor().GetLocation();
        float CursorLocalLocationZ = clientComponent->GetComponentTransform().InverseTransformPosition(CursorWorldLocation).Z;


        InitialLocalLocation = clientComponent->GetRelativeLocation().Z;
        InitialCursorOffsetZ = CursorLocalLocationZ - InitialLocalLocation;
    }
    else
    {

        PrimaryComponentTick.SetTickFunctionEnable(state);
        if (clientComponent)
        {
            FVector OriginalLocation = clientComponent->GetRelativeLocation();
            OriginalLocation.Z = InitialLocalLocation; 
            clientComponent->SetRelativeLocation(OriginalLocation);
        }
    }

    isPressed = false;
  
}



void USliderButton::Press()
{
    UE_LOG(LogTemp, Log, TEXT("%s pressed!"), *GetName());
    OnPressed.Broadcast();
    isPressed = true;
}


void USliderButton::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!focusSelector || !clientComponent) // Added clientComponent check
        return;


    FVector CursorWorldLocation = focusSelector->Cursor().GetLocation();
    float CursorLocalLocationZ = clientComponent->GetComponentTransform().InverseTransformPosition(CursorWorldLocation).Z;

    float DesiredZ = CursorLocalLocationZ - InitialCursorOffsetZ;

    DesiredZ = FMath::Clamp(DesiredZ, MaxDepressionDistance, InitialLocalLocation);

    
    FVector CurrentLocalLocation = clientComponent->GetRelativeLocation();
    float CurrentZ = CurrentLocalLocation.Z;

    const float LerpSpeed = 10.0f;
    float SmoothedZ = FMath::Lerp(CurrentZ, DesiredZ, FMath::Clamp(LerpSpeed * DeltaTime, 0.0f, 1.0f));

    FVector NewLocalLocation = CurrentLocalLocation;
    NewLocalLocation.Z = SmoothedZ;
    clientComponent->SetRelativeLocation(NewLocalLocation); 

   
    float CurrentDepression = SmoothedZ - InitialLocalLocation;

    if (CurrentDepression <= PressThreshold && !isPressed)
    {

        
        if (timer)
        {
            ABrickSpacePawn* BrickPawn = Cast<ABrickSpacePawn>(focusSelector->GetOwner());
            if (!BrickPawn) return;

            if (clientComponent == StartButton)
            {
                UE_LOG(LogTemp, Log, TEXT("StartButton pressed!"));
                timer->StartTimer(BrickPawn);

            }
            else if (clientComponent == LaunchButton)
            {
                UE_LOG(LogTemp, Log, TEXT("LaunchButton pressed!"));
                timer->StopTimer(BrickPawn);
                Press();
            }
            else if (clientComponent == ResetButton)
            {
                UE_LOG(LogTemp, Log, TEXT("ResetButton pressed!"));
                timer->ResetTimer(BrickPawn);
            }
            else if (clientComponent == PauseButton)
            {
                UE_LOG(LogTemp, Log, TEXT("PauseButton pressed!"));
                timer->StopTimer(BrickPawn);
                
            }
            UE_LOG(LogTemp, Warning, TEXT("Depression: %f"), CurrentDepression);
        }
    }
}