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

// Called when the game starts or when spawned
//void APainterActor::BeginPlay()
//{
//	Super::BeginPlay();
//	
//}

void APainterActor::Server_Move_Implementation(const FTransform& InitialTransform)
{
	SetActorTransform(InitialTransform);
}

//// Called every frame
//void APainterActor::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//}

