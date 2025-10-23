// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BrickSpacePlayerState.h"
#include "HandSelector.h"
class UWallBrick;
class UTimeManager;
class UWallMover;
class USliderButton;
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
	void Server_MoveRelative(USceneComponent* TargetActor, const FTransform& InitialTransform);

	UFUNCTION(Server, Reliable)
	void Server_Translate(UWallMover* TargetActor, float pct);

	UFUNCTION(Server, Reliable)
	void Server_Rotate(USceneComponent* TargetActor, const FRotator& Rot);

	UFUNCTION(Server, Reliable)
	void Server_Delete(AActor* TargetActor);

	UFUNCTION(Server, Reliable)
	void Server_CloneWallBrick(UWallBrick* wallBrick, const FTransform& onWallTransform);

	UFUNCTION(Server, Reliable)
	void Server_TryAdvanceLayer(UBrick* assemblyBrick);

	UFUNCTION(Server, Reliable)
	void Server_MeshScaleUpdate(USceneComponent* leftHandMesh, USceneComponent* rightHandMesh, float handScale);

	UFUNCTION(Server, Reliable)
	void Server_StartStopTimer(UTimeManager* timeManager, bool isRunning);

	UFUNCTION(Server, Reliable)
	void Server_ResetTimer(UTimeManager* timeManager);

	UFUNCTION(NetMulticast, Reliable)
	void ActivateParticleSystem(bool isActive);

	UFUNCTION(Server, Reliable)
	void Server_HandleLaunchButtonPress(USliderButton* ButtonComponent);


public:

	void VARLog(FString methodName);


};
