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

    InitialLocalLocationStart = StartButton->GetRelativeLocation();
    InitialLocalLocationLaunch = LaunchButton->GetRelativeLocation();
    InitialLocalLocationReset = ResetButton->GetRelativeLocation();
}

void USliderButton::Focus(USelector* cursor, bool state)
{
    Super::Focus(cursor, state);

 
    if (!StartButton || !LaunchButton || !ResetButton) return;

    ABrickSpacePawn* BrickPawn = nullptr;
    if (cursor)
    {
        BrickPawn = Cast<ABrickSpacePawn>(cursor->GetOwner());
    }

    if (state) // Cursor is focusing/touching the button
    {
        if (cursor)
        {

            if (StartButton) // This is the Start button instance
            {
                InitialLocalLocation = InitialLocalLocationStart;
            }
            else if (LaunchButton) // This is the Launch/Stop button instance
            {
                InitialLocalLocation = InitialLocalLocationLaunch;
            }
            else if (ResetButton) // This is the Reset button instance
            {
                InitialLocalLocation = InitialLocalLocationReset;
            }

            // 1. Get the cursor's position (using root component as the button's parent frame of reference)
            FVector CursorWorldLocation = cursor->Cursor().GetLocation();;
            FVector CursorLocalLocation = GetComponentTransform().InverseTransformPosition(CursorWorldLocation);

            // 2. Calculate the depression amount (assuming Z-axis travel)
            float Depression = FMath::Max(0.0f,  InitialLocalLocation.Z - CursorLocalLocation.Z);

            // 3. Clamp the depression to the max travel distance
            Depression = FMath::Min(Depression, MaxDepressionDistance);

            // 4. Set the new location
            FVector NewLocalLocation = InitialLocalLocation;
            NewLocalLocation.Z -= Depression;
         /*   SetButtonLocation(NewLocalLocation);*/

            // 5. Check for the Press event threshold and trigger timer action
            if (Depression >= PressThreshold && !isPressed)
            {
                Press();

                if (timer && BrickPawn)
                {
                    if (StartButton) // This is the Start button instance
                    {   
                        StartButton->SetRelativeLocation(NewLocalLocation);
                        timer->StartTimer(BrickPawn);
                    }
                    else if (LaunchButton) // This is the Launch/Stop button instance
                    {
                        LaunchButton->SetRelativeLocation(NewLocalLocation);
                        timer->StopTimer(BrickPawn);
                    }
                    else if (ResetButton) // This is the Reset button instance
                    {
                        ResetButton->SetRelativeLocation(NewLocalLocation);
                        timer->ResetTimer(BrickPawn);
                    }
                }
            }
        }
    }

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
}