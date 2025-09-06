// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
class UBrick;
#include "BrickActor.generated.h"

UCLASS()
class BRICKSPACE_API ABrickActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABrickActor();

	// Changeing material
	UFUNCTION(Server, Reliable)
	void Server_ChangeMaterial(ABrickActor* TargetActor, UMaterialInterface* material, bool brickIsSolid);

	UBrick* brick;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
