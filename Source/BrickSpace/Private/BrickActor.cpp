// Fill out your copyright notice in the Description page of Project Settings.


#include "BrickActor.h"
#include "Brick.h"
#include "WallBrick.h"
#include "CoreMinimal.h"

// Sets default values
ABrickActor::ABrickActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	//bReplicates = true;
	SetReplicates(true);
}

// Called when the game starts or when spawned
void ABrickActor::BeginPlay()
{
	Super::BeginPlay();
	brick = FindComponentByClass<UBrick>();
}

//void ABrickActor::Server_Clone_Implementation(const FTransform& InitialTransform)
//{
//	GetWorld()->SpawnActor<AActor>(GetClass(), InitialTransform );
//
//	// When we set this from the server it will replicate to all clients.
//	UWallBrick* wallBrick = FindComponentByClass<UWallBrick>();
//	if (wallBrick != nullptr)
//		wallBrick->bThresholdReached = true;
//}


void ABrickActor::Server_Delete_Implementation()
{
	Destroy(true, true);
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

void ABrickActor::Server_Move_Implementation(AActor* TargetActor, const FTransform& InitialTransform)
{
	TargetActor->SetActorTransform(InitialTransform);
}