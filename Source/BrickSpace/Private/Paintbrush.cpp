// Fill out your copyright notice in the Description page of Project Settings.


#include "Paintbrush.h"
#include "BrickActor.h"
#include "PainterActor.h"
#include "Selector.h"
#include "BrickSpacePawn.h"
//#include <Kismet/GameplayStatics.h>
#include "BrickSpacePlayerState.h"

void UPaintbrush::BeginPlay()
{
	Super::BeginPlay();
	selectionFilter = 0x02; // Only Painter can grab
}

void UPaintbrush::ChangeBrickTouched()
{
	//if (playerState != nullptr && GetOwner() != nullptr && brickTouched != nullptr) {
	//	UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(brickTouched->GetAttachParent());
	//	if (MeshComp != nullptr && MeshComp->Mobility == EComponentMobility::Movable)
	//	{
	//		//MeshComp->SetMaterial(0, brushMaterial);
	//		playerState->Server_ChangeMaterial(brickTouched->GetOwner(), brushMaterial, true);
	//	}
	//}

	if (brickTouched && brickTouched->GetOwner() ) {
		ABrickActor* brickTouchedActor = Cast<ABrickActor>(brickTouched->GetOwner());
		if (brickTouchedActor) {
			UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(brickTouched->GetAttachParent());
			if (MeshComp != nullptr && MeshComp->Mobility == EComponentMobility::Movable)
				brickTouchedActor->Server_ChangeMaterial(brushMaterial, true);
		}
	}
}


void UPaintbrush::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	// States: 
	// Seeking: No overlap with other bricks, normal grabbing mode only.
	// Snapping: Normal grabbing mode followed by tube/stud proximity matches.
	// PinSnapped: Unsnap stress test. If fail: Enter Snapping/Seeking mode. Otherwise: Pin rotate followed by tube/stud proximity matches.
	// RigidSnapped: Unsnap stress test only.

	// Move brick freely, if already snapped the brick will temporarilly be returned to unsnapped movement to allow unsnap testing.
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	APainterActor* painterActor = Cast<APainterActor>(GetOwner());
	painterActor->Server_Move(clientComponent->GetComponentTransform());

	//if (playerState != nullptr && GetOwner() != nullptr) {
	//	playerState->Server_MoveActor(clientComponent->GetOwner(), clientComponent->GetComponentTransform());
	//}
}

void UPaintbrush::UpdateColor(USelector* selector)
{
	AActor* owner = selector->GetOwner();
	ABrickSpacePawn* pawn = Cast<ABrickSpacePawn>(owner);
	pawn->UpdateHandColor(brushMaterial, selector);
}
