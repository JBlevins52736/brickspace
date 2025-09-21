// Fill out your copyright notice in the Description page of Project Settings.

#include "Paintbrush.h"
#include "BrickActor.h"
#include "PainterActor.h"
#include "Selector.h"
#include "BrickSpacePawn.h"
#include <Kismet/GameplayStatics.h>
#include "BrickSpacePlayerState.h"

void UPaintbrush::BeginPlay()
{
	Super::BeginPlay();
	selectionFilter = 0x02; // Only Painter can grab
}

void UPaintbrush::ForePinch(USelector* selector, bool state)
{
	if (clientComponent->Mobility != EComponentMobility::Movable &&
		grabbingSelector != nullptr && grabbingSelector != selector)
		return;

	Super::ForePinch(selector, state);

	// Change the hand material to brushMaterial on grab
	UHandSelector* handSelector = Cast<UHandSelector>(selector);
	if (handSelector && state)
		handSelector->SetMaterial(brushMaterial);
}

void UPaintbrush::ChangeBrickTouched()
{
	// Change the brickTouched material to brushMaterial when the paintbrush tool overlaps it.
	if (brickTouched && brickTouched->GetOwner()) {
		ABrickActor* brickTouchedActor = Cast<ABrickActor>(brickTouched->GetOwner());
		if (brickTouchedActor) {
			UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(brickTouched->GetAttachParent());
			if (MeshComp != nullptr && MeshComp->Mobility == EComponentMobility::Movable) {
				if (playerState == nullptr) {
					APlayerState* PlayerStateAtIndex0 = UGameplayStatics::GetPlayerState(GetWorld(), 0);
					playerState = Cast<ABrickSpacePlayerState>(PlayerStateAtIndex0);
				}
				playerState->Server_ChangeMaterial(brickTouchedActor, brushMaterial, true);
			}
		}
	}
}


void UPaintbrush::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	APainterActor* painterActor = Cast<APainterActor>(GetOwner());
	painterActor->Server_Move(clientComponent->GetComponentTransform());
}
