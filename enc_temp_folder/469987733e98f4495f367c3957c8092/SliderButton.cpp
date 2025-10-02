#include "SliderButton.h"
#include "BrickSpacePawn.h"

#include "Selector.h"

USliderButton::USliderButton()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void USliderButton::BeginPlay()
{
    Super::BeginPlay();

}
void USliderButton::Focus(USelector* cursor, bool state)
{
    // Let parent handle focus registration
    Super::Focus(cursor, state);
    
    ABrickSpacePawn* BrickPawn = nullptr;

    if (cursor)
    {
        AActor* OwnerActor = cursor->GetOwner();
        if (OwnerActor)
        {
            BrickPawn = Cast<ABrickSpacePawn>(OwnerActor);
        }
    }

    if (!LaunchButton && !StartButton) return;

    if (state)
    {     // Move to pressed target
        if (LaunchButton) 
        {
            Press();
            if (timer) 
            {
                timer->StopTimer(BrickPawn);
            }
        }
        else if (StartButton)
        {
            Press();
            if (timer)
            {
                timer->StartTimer(BrickPawn);
            }
        }
    }
    else
    {     
        Release();
    }
}

void USliderButton::Press()
{
    UE_LOG(LogTemp, Log, TEXT("%s pressed!"), *GetName());
    OnPressed.Broadcast(); // Call Blueprint-bound event
}

void USliderButton::Release()
{


    UE_LOG(LogTemp, Log, TEXT("%s released!"), *GetName());
    OnReleased.Broadcast(); // Call Blueprint-bound event
}

void USliderButton::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}