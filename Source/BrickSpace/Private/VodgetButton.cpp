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

    if (state)
    {
        // Button pressed
        bIsPressed = true;

        if (bIsToggle)
        {
            bIsOn = !bIsOn;
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