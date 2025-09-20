// Fill out your copyright notice in the Description page of Project Settings.


#include "Grabber.h"
#include "Selector.h"
#include "BrickSpacePlayerState.h"

#include <Kismet/GameplayStatics.h>

void UGrabber::Focus(USelector* selector, bool state)
{
	// Optional: Change some property of the clients material to highlight when focused. 
	UVodget::Focus(selector, state);
}

void UGrabber::ForePinch(USelector* selector, bool state)
{
	if (clientComponent->Mobility != EComponentMobility::Movable &&
		grabbingSelector != nullptr && grabbingSelector != selector)
		return;	
	
	if (state && ! GetOwner()->HasAuthority() ) {
		APlayerState* PlayerStateAtIndex0 = UGameplayStatics::GetPlayerState(GetWorld(), 0);
		ABrickSpacePlayerState* playerState = Cast<ABrickSpacePlayerState>(PlayerStateAtIndex0);

		playerState->Server_Own(GetOwner(), selector->GetOwner());
	}

	// We grab the selectors focus when grabbing is true to ensure receiving focusUpdate until grabbing is released.
	selector->GrabFocus(state);
	if (state) {

		//UE_LOG(LogTemp, Warning, TEXT("Grab:%s"), *FString(GetOwner()->GetActorLabel()));
		grabbingSelector = selector;

		// Set childsrt to the clients transform as a child of the selectors cursor.
		childsrt = clientComponent->GetComponentTransform() * selector->Cursor().Inverse();
		UpdateColor(selector);
	}
	else {
		//UE_LOG(LogTemp, Warning, TEXT("Release:%s"), *FString(GetOwner()->GetActorLabel()));
		grabbingSelector = nullptr;
	}

	// Use PrimaryComponentTick.SetTickFunctionEnable to turn tick on when grabbing and off when not grabbing.
	PrimaryComponentTick.SetTickFunctionEnable(state);

}

void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	// ToDo: While grabbing operation is active convert the childsrt that was set when grabbing began back
	// to world space and set the clients world transform to the result.
	if (grabbingSelector != nullptr) {
		//UE_LOG(LogTemp, Warning, TEXT("Grabber FocusUpdate:%s"), *FString(GetOwner()->GetActorLabel()));
		FTransform worldsrt = childsrt * grabbingSelector->Cursor();
		clientComponent->SetWorldTransform(worldsrt);
	}
}

void UGrabber::UpdateColor(USelector* selector)
{
	// Use this to default and set brick color
	
	//UE_LOG(LogTemp, Warning, TEXT("Used grabber version"));
}
