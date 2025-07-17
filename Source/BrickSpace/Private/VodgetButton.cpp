#include "VodgetButton.h"
#include "Selector.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

void UVodgetButton::Focus(USelector* cursor, bool state)
{
    // Call parent class implementation
    UVodget::Focus(cursor, state);
}

void UVodgetButton::ForePinch(USelector* selector, bool state)
{
    if (!selector) return;

    if (state) {
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
    

}

