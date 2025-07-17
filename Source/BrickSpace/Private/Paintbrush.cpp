// Fill out your copyright notice in the Description page of Project Settings.


#include "Paintbrush.h"

void UPaintbrush::BeginPlay()
{
	Super::BeginPlay();
	selectionFilter = 0x02; // Only Painter can grab
}

void UPaintbrush::ChangeBrickTouched()
{
	UStaticMeshComponent* mesh = Cast<UStaticMeshComponent>(brickTouched->GetAttachParent());
	if (mesh != nullptr)
	{
		// Add overlap with bricks.
		mesh->SetMaterial(0, brushMaterial);
	}
}
