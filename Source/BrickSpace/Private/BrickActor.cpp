// Fill out your copyright notice in the Description page of Project Settings.


#include "BrickActor.h"
#include "Brick.h"

// Sets default values
ABrickActor::ABrickActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	bReplicates = true;
	//SetReplicates(true);
}

// Called when the game starts or when spawned
void ABrickActor::BeginPlay()
{
	Super::BeginPlay();
	brick = FindComponentByClass<UBrick>();
}

void ABrickActor::Server_ChangeMaterial_Implementation(UMaterialInterface* material, bool brickIsSolid)
{
	if (brick)
	{
		brick->brickMaterial = material;
		brick->isSolid = brickIsSolid;
		brick->OnRep_Material();
	}
}
