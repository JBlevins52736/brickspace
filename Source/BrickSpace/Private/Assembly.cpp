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

}

// Called when the game starts
void UAssembly::BeginPlay()
{
	Super::BeginPlay();

	// Find the groundplate bricks before snapped bricks might be added to the hierarchy.
	GetOwner()->GetComponents<UBrick>(groundPlateBricks);
}

void UAssembly::LoadAssembly(FString fname)
{
}

void UAssembly::SaveAssembly(FString fname)
{
	//{
	//	// 
	//	for (int i = 0; i < groundPlateBricks.Num(); ++i)
	//		groundPlateBricks[i]->selectionFilter = 0xFF;
	//}

	int layer = 0;
	int bricksAdded = 0;
	do {
		bricksAdded = 0;
		for (int i = 0; i < groundPlateBricks.Num(); ++i)
			groundPlateBricks[i]->FindLayerBricks(layer);
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
