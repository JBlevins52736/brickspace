// Fill out your copyright notice in the Description page of Project Settings.

#include "BrickSpacePlayerState.h"

#include "WallBrick.h"

void ABrickSpacePlayerState::Server_CloneActor_Implementation(AActor* TargetActor, const FTransform& InitialTransform)
{
	AActor* Replacement = GetWorld()->SpawnActor<AActor>(
		TargetActor->GetClass(),
		InitialTransform
	);

	UWallBrick* wallBrick = TargetActor->FindComponentByClass<UWallBrick>();
	if (wallBrick != nullptr)
		wallBrick->bThresholdReached = true;
}

void ABrickSpacePlayerState::Server_MoveActor_Implementation(AActor* TargetActor, const FTransform& InitialTransform)
{
	//clientComponent->SetWorldTransform(worldsrt);
	TargetActor->SetActorTransform(InitialTransform);
}

void ABrickSpacePlayerState::Server_ChangeMaterial_Implementation(AActor* TargetActor, UMaterialInterface* material, bool brickIsSolid)
{
	UBrick* brick = TargetActor->FindComponentByClass<UBrick>();
	if (brick != nullptr) {
		brick->brickMaterial = material;
		brick->isSolid = brickIsSolid;
		brick->OnRep_Material();
	}
}

void ABrickSpacePlayerState::Server_ChangeGrabbable_Implementation(AActor* TargetActor, bool isGrabbable)
{
	UBrick* brick = TargetActor->FindComponentByClass<UBrick>();
	if (brick != nullptr) {
		brick->isGrabbable = isGrabbable;
		brick->OnRep_Grabbable();
	}
}
