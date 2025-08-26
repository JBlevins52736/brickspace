// Fill out your copyright notice in the Description page of Project Settings.


#include "Paintbrush.h"
#include <Kismet/GameplayStatics.h>

void UPaintbrush::BeginPlay()
{
	Super::BeginPlay();
	selectionFilter = 0x02; // Only Painter can grab
}

void UPaintbrush::ChangeBrickTouched()
{
	if (playerState != nullptr && GetOwner() != nullptr && brickTouched != nullptr ) {
		UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(brickTouched->GetAttachParent());
		if (MeshComp != nullptr && MeshComp->Mobility == EComponentMobility::Movable)
		{
			//MeshComp->SetMaterial(0, brushMaterial);
			playerState->Server_ChangeMaterial(brickTouched->GetOwner(), brushMaterial, true );
		}
	}
}
