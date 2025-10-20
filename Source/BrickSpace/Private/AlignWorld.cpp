// Fill out your copyright notice in the Description page of Project Settings.


#include "AlignWorld.h"
#include "Selector.h"

void UAlignWorld::ForePinch(USelector* selector, bool state)
{
	selector->GrabFocus(state);
	if (state)
	{
		grabbingSelector = selector;
		childsrt = clientComponent->GetComponentTransform() * selector->Cursor().Inverse();
	}
	else {
		grabbingSelector = nullptr;
	}
	PrimaryComponentTick.SetTickFunctionEnable(state);	
	
}

void UAlignWorld::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (grabbingSelector) {
		FTransform worldsrt = childsrt * grabbingSelector->Cursor();
		AlignDelegate.Broadcast(worldsrt.GetLocation());
	
	}
}
