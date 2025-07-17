// Fill out your copyright notice in the Description page of Project Settings.


#include "RevealWand.h"

void URevealWand::BeginPlay()
{
	Super::BeginPlay();

	selectionFilter = 0x04; // Only Architect can grab
}

void URevealWand::ChangeBrickTouched()
{
	// Reveal brickTouched.
	UStaticMeshComponent* mesh = Cast<UStaticMeshComponent>(brickTouched->GetAttachParent());
	if (mesh != nullptr && mesh->Mobility == EComponentMobility::Movable)
	{
		// Add overlap with bricks.
		mesh->SetMaterial(0, revealMaterial);
	}
}
