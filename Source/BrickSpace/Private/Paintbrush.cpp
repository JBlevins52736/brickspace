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

				// Brick is either being grabbed and collided with the brush or the brush is being grabbed and collided with the brick.
				USelector* selector = (grabbingSelector) ? grabbingSelector : brickTouched->GetGrabbingSelector();
				if (selector)
				{
					ABrickSpacePawn* pawn = Cast<ABrickSpacePawn>(selector->GetOwner());
					if (pawn) {
						if (pawn->HasAuthority()) {
							brickTouched->Server_ChangeMaterial(brushMaterial, true);
						}
						else {
							pawn->Server_ChangeMaterial(brickTouched, brushMaterial, true);
						}
					}
				}

			}
		}
	}
}


void UPaintbrush::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (grabbingSelector == nullptr)
		return;

	// Grabber base class has already moved the paintbrush tool.
	// If moved on the listen server client, nothing more needs to be done.
	// If moved on a remote client, the server needs to be notified.
	ABrickSpacePawn* pawn = Cast<ABrickSpacePawn>(grabbingSelector->GetOwner());
	if (!pawn->HasAuthority())
		pawn->Server_Move(GetOwner(), clientComponent->GetComponentTransform());
}
