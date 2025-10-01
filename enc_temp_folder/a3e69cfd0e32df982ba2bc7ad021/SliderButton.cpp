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
    if (LaunchButton && StartButton)
    {
        Launch = LaunchButton->GetRelativeLocation();
        Start = StartButton->GetRelativeLocation();
    }
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
    {
        // Move to pressed target
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

    
    if (LaunchButton)
    {
        Launch = Launch + downPush;
    }
    else if (StartButton)
    {
        Start = Start + downPush;
    }
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

    if (StartButton)
    {
        FVector Current = StartButton->GetRelativeLocation();
        FVector NewPos = FMath::VInterpTo(Current, downPush, DeltaTime, 10.0f);
        StartButton->SetRelativeLocation(NewPos);
        UE_LOG(LogTemp, Log, TEXT("%s pressed!"), *NewPos.ToString());
    }
    else if (LaunchButton)
    {
     
       FVector Current = LaunchButton->GetRelativeLocation();
       FVector NewPos = FMath::VInterpTo(Current, downPush, DeltaTime, 10.0f);
       LaunchButton->SetRelativeLocation(NewPos);
       UE_LOG(LogTemp, Log, TEXT("%s pressed!"), *NewPos.ToString());
    
    }
}