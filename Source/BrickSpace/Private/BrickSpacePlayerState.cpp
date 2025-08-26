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
	//UStaticMeshComponent* MeshComp = TargetActor->FindComponentByClass<UStaticMeshComponent>();
	//if (MeshComp)
	//{
	//	MeshComp->SetMaterial(0, material);
		UBrick* brick = TargetActor->FindComponentByClass<UBrick>();
		if (brick != nullptr) {
			brick->brickMaterial = material;
			brick->isSolid = brickIsSolid;
		}
	//}
}

void ABrickSpacePlayerState::Server_ChangeGrabbable_Implementation(AActor* TargetActor, bool isGrabbable)
{
	UBrick* brick = TargetActor->FindComponentByClass<UBrick>();
	if (brick != nullptr) {
		brick->isGrabbable = isGrabbable;
	}
}
