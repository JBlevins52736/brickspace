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

    if (!LaunchButton && !StartButton && !ResetButton) return;

    if (state)
    {    
        if (LaunchButton && !isPressed) 
        {
            Press();
            if (timer) 
            {
                timer->StopTimer(BrickPawn);
            }
        }
        else if (StartButton && !isPressed)
        {
            Press();
            if (timer)
            {
                timer->StartTimer(BrickPawn);
            }
        }
        else if (ResetButton && !isPressed)
        {
            Press();
            if (timer)
            {
                timer->ResetTimer(BrickPawn);
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
    OnPressed.Broadcast();
    isPressed = true;
}


void USliderButton::Release()
{
 
    if (GetWorld()->GetTimerManager().IsTimerActive(ReleaseDelayHandle))
        return;

    if (GetWorld())
    {
        GetWorld()->GetTimerManager().SetTimer(
            ReleaseDelayHandle, this, &USliderButton::DoRelease, ReleaseDelayDuration, false
        );
    }
}

void USliderButton::DoRelease()
{
    UE_LOG(LogTemp, Log, TEXT("%s released after delay!"), *GetName());
    OnReleased.Broadcast();
    isPressed = false; 
}

void USliderButton::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}