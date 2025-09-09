// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Assembly.h"

#include "BrickSpacePlayerState.generated.h"

/**
 * 
 */
UCLASS()
class BRICKSPACE_API ABrickSpacePlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	UFUNCTION(Server, Reliable)
	void Server_CloneActor(AActor* TargetActor, const FTransform& InitialTransform);

	UFUNCTION(Server, Reliable)
	void Server_MoveActor(AActor* TargetActor, const FTransform& InitialTransform);

	UFUNCTION(Server, Reliable)
	void Server_DeleteActor(AActor* TargetActor);

	UFUNCTION(Server, Reliable)
	void Server_ChangeMaterial(AActor* TargetActor, UMaterialInterface* material, bool isSolid );

	UFUNCTION(Server, Reliable)
	void Server_ChangeGrabbable(AActor* TargetActor, bool isGrabbable);

	UFUNCTION(Server, Reliable)
	void Server_TryAdvanceLayer(AActor* GroundplateActor);

	UFUNCTION(Server, Reliable)
	void Server_ChangeHandColor(AActor* target, UMaterialInterface* material);

	UFUNCTION(Server, Reliable)
	void Server_UpdatePlayerHandPos(AActor* target, FTransform leftTransform, FTransform rightTransform);

};
