#include "Vodget.h"
#include "Selector.h"

// Sets default values for this component's properties
UVodget::UVodget()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

// Called when the game starts
void UVodget::BeginPlay()
{
	Super::BeginPlay();

	if (clientComponent == nullptr)
	{
		// Vodgets are scenecomponents that are typically attached as children to manipulate their
		// parent scenecomponent or, if they are children of world space they operate on themselves.
		clientComponent = GetAttachParent();
		if (clientComponent == nullptr)
			clientComponent = this;
	}
}

// Selectors call Vodgets Focus method to notify when a Vodget recieves or loses the selectors focus.
void UVodget::Focus(USelector* cursor, bool state)
{
	//PrimaryComponentTick.SetTickFunctionEnable(state)
}

// Selectors forward all ForePinch events to the Vodget that currently has its focus.
void UVodget::ForePinch(USelector* cursor, bool state)
{
}

// Selectors forward all MiddlePinch events to the Vodget that currently has its focus.
void UVodget::MiddlePinch(USelector* cursor, bool state)
{
}

// Selectors forward all Grip events to the Vodget that currently has its focus.
void UVodget::Grip(USelector* cursor, bool state)
{
}

FString UVodget::ClientName()
{
	return GetOwner()->GetName();
}
