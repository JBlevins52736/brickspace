// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BrickSpacePlayerState.h"
#include "HandSelector.h"
#include "BrickSpacePawn.generated.h"

UCLASS()
class BRICKSPACE_API ABrickSpacePawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABrickSpacePawn();

	UFUNCTION(Server, Reliable)
	void Server_ChangeMaterial(UBrick* TargetActor, UMaterialInterface* material, bool isSolid);

	UFUNCTION(Server, Reliable)
	void Server_Move(AActor* TargetActor, const FTransform& InitialTransform);

	UFUNCTION(Server, Reliable)
	void Server_Delete(AActor* TargetActor);

	UFUNCTION(Server, Reliable)
	void Server_TryAdvanceLayer(AAssemblyActor* assemblyActor);

public:

	void VARLog(FString methodName);

};
