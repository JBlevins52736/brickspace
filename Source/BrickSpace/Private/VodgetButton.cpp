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
	AActor* menuActor = Cast<AActor>(GetOwner());
	if (menuActor && !menuActor->GetRootComponent()->IsVisible()) return; // Check to see if the button should be interacted with. If its not visible, it should not be interacted with.
																		  // The assumption is, this type of button is being used on menus
	if (state) // The below is needed so that we are not, on release of the button sending updates to stop world grabbing
	{

		isToggle = !isToggle;
		OnButton.Broadcast(isToggle);
	}
	
}