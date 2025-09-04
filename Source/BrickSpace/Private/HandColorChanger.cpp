// Fill out your copyright notice in the Description page of Project Settings.


#include "HandColorChanger.h"
#include "HandSelector.h"
#include "BrickSpacePlayerState.h"
#include "BrickSpacePawn.h"
#include <Kismet/GameplayStatics.h>

void UHandColorChanger::Focus(USelector* selector, bool state)
{
	UVodget::Focus(selector, state);

	if (!selector->GetOwner())
		return;

	UHandSelector* handSelector = Cast<UHandSelector>(selector);
	//if (handSelector)
	//	handSelector->handMaterial = brushMaterial;

	//if (playerState == nullptr) {
	//	APlayerState* PlayerStateAtIndex0 = UGameplayStatics::GetPlayerState(GetWorld(), 0);
	//	playerState = Cast<ABrickSpacePlayerState>(PlayerStateAtIndex0);
	//	if (!playerState)
	//		return;
	//}

	//playerState->Server_ChangeMaterial(selector->GetOwner(), brushMaterial, true);
	AActor* actor = selector->GetOwner();

	ABrickSpacePawn* pawn = Cast<ABrickSpacePawn>(actor);
	TArray<UActorComponent*> actorComps;
	pawn->GetComponents(actorComps);
	for (int i = 0; i < actorComps.Max(); i++) {
		//UActorComponent* actor
	}
	// add in param to send to server to distinguish which selector needs color updated
	pawn->NotifyServerOfHandMatChange(selector, brushMaterial);

	
}