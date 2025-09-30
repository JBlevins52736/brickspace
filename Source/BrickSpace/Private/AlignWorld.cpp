// Fill out your copyright notice in the Description page of Project Settings.


#include "AlignWorld.h"
#include "Selector.h"

#include "WorldAlignmentActor.h"

void UAlignWorld::ForePinch(USelector* selector, bool state)
{
	selector->GrabFocus(state);
	grabbingSelector = (state) ? selector : nullptr;

	if (state) {
		UE_LOG(LogTemp, Warning, TEXT("AlignWorld button grabbed:"));
	} 
	else {
		UE_LOG(LogTemp, Warning, TEXT("AlignWorld button released:"));
	}


	PrimaryComponentTick.SetTickFunctionEnable(state);	
}

void UAlignWorld::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (grabbingSelector) {
		//AlignDelegate.Broadcast(grabbingSelector->Cursor().GetLocation());
		UE_LOG(LogTemp, Warning, TEXT("AlignWorld button broadcasting"));
	}
}
