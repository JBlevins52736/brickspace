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
	selector->GrabFocus(state);
	UChildActorComponent* childActor = Cast<UChildActorComponent>(GetOwner());
	if (childActor ) {
		GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Red, TEXT("pinchChanged"));
		return;
	}
	if (state) {

		isToggle = !isToggle;
		OnButton.Broadcast(isToggle);
	}
	
}