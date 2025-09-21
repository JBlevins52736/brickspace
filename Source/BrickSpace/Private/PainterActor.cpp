// Fill out your copyright notice in the Description page of Project Settings.


#include "PainterActor.h"
#include "CoreMinimal.h"

// Sets default values
APainterActor::APainterActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	bReplicates = true;
}

void APainterActor::Server_Move_Implementation(const FTransform& InitialTransform)
{
	SetActorTransform(InitialTransform);
}
