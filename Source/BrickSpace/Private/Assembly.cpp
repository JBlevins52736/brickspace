// Fill out your copyright notice in the Description page of Project Settings.


#include "Assembly.h"
#include "Brick.h"

UAssembly* UAssembly::_Instance = nullptr;

// Sets default values for this component's properties
UAssembly::UAssembly()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// THERE CAN BE ONLY ONE
	_Instance = this;

	// Find the groundplate bricks before snapped bricks might be added to the hierarchy.
	GetOwner()->GetComponents<UBrick>(bricks);
}

// Called when the game starts
void UAssembly::BeginPlay()
{
	Super::BeginPlay();
}

void UAssembly::LoadAssembly(FString fname)
{
}

int UAssembly::AddLayer(int layerind, UBrick* brick)
{
	return 0;
}

void UAssembly::SaveAssembly(FString fname)
{
	int layer = 0;
	int bricksAdded = 0;
	do {
		bricksAdded = 0;
		for (int i = 0; i < bricks.Num(); ++i)
			bricksAdded += AddLayer(layer, bricks[i] );
	} while (bricksAdded > 0);
}

bool UAssembly::PlayMode()
{
	return false;
}

bool UAssembly::TryAddBrick(UBrick* brick)
{
	return false;
}

bool UAssembly::TryRemoveBrick(UBrick* brick)
{
	return false;
}
