// Fill out your copyright notice in the Description page of Project Settings.


#include "Assembly.h"
#include "Brick.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "Misc/FileHelper.h"
#include "CoreMinimal.h"


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

void UAssembly::LoadAssembly()
{
	// Load a file into an FString
	FString jsonFilePath;

	FString FileContent;
	if (FFileHelper::LoadFileToString(FileContent, *jsonFilePath))
	{
		TArray<FAssemblyBrick> data;
		FJsonObjectConverter::JsonArrayStringToUStruct(FileContent, &data, 0, 0);

		UE_LOG(LogTemp, Warning, TEXT("File content: %s"), *FileContent);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load file."));
	}

}

void UAssembly::SaveAssembly(const int Value)
{
	//if (!Value)
	//	return;

	FAssemblyBrickList brickList;

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

		// Add all reparented bricks to FAssemblyBrick list to be saved/serialized as JSON.
		for (int i = 0; i < reparentedBricks.size(); ++i) {

			brickList.bricks.Add(FAssemblyBrick{ 
				layer,
				reparentedBricks[i]->shortName,
				reparentedBricks[i]->GetLocation(),
				reparentedBricks[i]->GetQuat(),
				reparentedBricks[i]->GetMaterialPathName() });
		}

		layerBricks = reparentedBricks;
		reparentedBricks.clear();
		++layer;
	};

	// Save bricks vector to json file.
	TSharedPtr<FJsonObject> JsonObject = FJsonObjectConverter::UStructToJsonObject(brickList);
	if (JsonObject != nullptr)
	{
		FString JsonString;
		if (!FJsonSerializer::Serialize(JsonObject.ToSharedRef(), TJsonWriterFactory<>::Create(&JsonString, 0)))
		{
			// ERROR
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("JSON:%s"), *JsonString);

#ifdef BLAH
			FString jsonFilePath;
			if (FFileHelper::SaveStringToFile(JsonString, *jsonFilePath))
			{
				UE_LOG(LogTemp, Warning, TEXT("String saved to file successfully."));
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to save string to file."));
			}
#endif
		}

	}
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
