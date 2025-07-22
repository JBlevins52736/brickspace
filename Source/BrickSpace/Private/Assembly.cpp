// Fill out your copyright notice in the Description page of Project Settings.


#include "Assembly.h"
#include "Brick.h"

USTRUCT()
struct FBrick {
public:

	UPROPERTY()
	int layerInd;

	UPROPERTY()
	FString shortName;

	UPROPERTY()
	FVector position;

	UPROPERTY()
	FQuat rotation;

	UPROPERTY()
	FString materialPathName;
};

// Sets default values for this component's properties
UAssembly::UAssembly()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
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
	std::vector<FBrick> bricks;

	// Populate layerBricks with ground plate bricks as first (untracked) layer.
	std::vector<UBrick*> layerBricks;
	for (int i = 0; i < groundPlateBricks.Num(); ++i)
		layerBricks.push_back(groundPlateBricks[i]);
	int layer = 0;
	while (layerBricks.size() > 0)
	{
		// Change all connected bricks parent to the assembly sceneComponent.
		// Returns only the connected bricks that were reparented as the next assembly layer.
		std::vector<UBrick*> reparentedBricks;
		for (int i = 0; i < layerBricks.size(); ++i)
			layerBricks[i]->ReparentConnectedBricks(this, reparentedBricks);

		// Add all reparented bricks to FBrick list to be saved/serialized as JSON.
		for (int i = 0; i < reparentedBricks.size(); ++i) {
			FBrick brick;
			brick.layerInd = layer;
			brick.shortName = layerBricks[i]->shortName;
			brick.position = layerBricks[i]->GetLocation();
			brick.rotation = layerBricks[i]->GetQuat();
			brick.materialPathName = layerBricks[i]->GetMaterialPathName();
			bricks.push_back(brick);
		}

		layerBricks = reparentedBricks;
		++layer;
	};

	// Save bricks vector to json file.

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
