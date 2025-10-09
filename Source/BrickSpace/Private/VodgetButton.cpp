#include "VodgetButton.h"
#include "Selector.h"

void UVodgetButton::BeginPlay()
{
    Super::BeginPlay();
    selectionFilter = 0x03; // Only Manufacturer can grab bricks
}

void UVodgetButton::Focus(USelector* cursor, bool state)
{
    // Call parent class implementation
    UVodget::Focus(cursor, state);
}

void UVodgetButton::ForePinch(USelector* selector, bool state)
{
    if (!selector) return;

    if (state && !bIsPressed)
    {
        // Button pressed
        bIsPressed = true;

        if (bIsToggle)
        {
            bIsOn = !bIsOn;
            GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Blue, TEXT("Button pressed"));
            OnToggled.Broadcast(ButtonID, bIsOn);
        }
        else
        {
            OnPressed.Broadcast(ButtonID);
        }
    }
    else
    {
        // Button released
        bIsPressed = false;
    }
}