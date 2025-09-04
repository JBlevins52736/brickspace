// Fill out your copyright notice in the Description page of Project Settings.

#include "BrickSpacePlayerState.h"
#include "HandSelector.h"
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

void ABrickSpacePlayerState::Server_DeleteActor_Implementation(AActor* TargetActor)
{
	TargetActor->Destroy(true, true);
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

void ABrickSpacePlayerState::Server_TryAdvanceLayer_Implementation(AActor* GroundplateActor)
{
	UAssembly* assembly = GroundplateActor->FindComponentByClass<UAssembly>();
	if (assembly != nullptr) {
		assembly->TryAdvanceLayer();
	}
}

void ABrickSpacePlayerState::Server_ChangeHandColor_Implementation(AActor* target, UMaterialInterface* material)
{
	TArray<UActorComponent*> actorComp;
	target->GetComponents(actorComp);
	// this will need adjustment because its going to look for the first one and that is it
	for (UActorComponent* actor : actorComp) {
		UHandSelector* selector = Cast<UHandSelector>(actor);
		if (selector) {
			selector->handMaterial = material;
			selector->OnRep_Material();
			break;
		}
	}
}

void ABrickSpacePlayerState::Server_UpdatePlayerHandPos_Implementation(AActor* target, FTransform rightTransform, FTransform leftTransform)
{
	TArray<UPrimitiveComponent*> actorComps;
	target->GetComponents(actorComps);
	//bool firstHandFound = false;
	for (UPrimitiveComponent* actorComp : actorComps) {
		if (actorComp->GetName().Contains("LMotionController")) {
			USceneComponent* hand = actorComp->GetChildComponent(0);
			hand->SetWorldTransform(leftTransform);
			GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Orange, TEXT("Entered the server function"));
		}
		else if (actorComp->GetName().Contains("RMotionController")) {
			USceneComponent* hand = actorComp->GetChildComponent(0);
			hand->SetWorldTransform(rightTransform);
		}
	}
}
