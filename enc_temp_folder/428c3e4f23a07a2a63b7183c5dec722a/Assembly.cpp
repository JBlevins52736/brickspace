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
	/// HACK: Still loading from PC... must load from game content folder.

	// Load a file into an FString
	FString file = FPaths::ProjectConfigDir();
	file.Append(TEXT("Rocket.json"));

	// We will use this FileManager to deal with the file.
	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();

	FString JsonString;
	// Always first check if the file that you want to manipulate exist.
	if (FileManager.FileExists(*file))
	{
		// We use the LoadFileToString to load the file into
		if (FFileHelper::LoadFileToString(JsonString, *file, FFileHelper::EHashOptions::None))
		{
			UE_LOG(LogTemp, Warning, TEXT("FileManipulation: Text From File: %s"), *JsonString);

			// Create a TSharedPtr to hold the FJsonObject
			TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());

			// Create a TJsonReader to read the JsonString
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

			// Attempt to deserialize the JSON string into the FJsonObject
			if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
			{
				// Successfully deserialized the JSON string into JsonObject
				// You can now access the fields of the JsonObject
				//FString Name = JsonObject->GetStringField(TEXT("name"));
				//FString Version = JsonObject->GetStringField(TEXT("version"));

				FAssemblyBrickList bricklist;
				if (FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &bricklist, 0, 0))
				{
					// MyConvertedData now contains the data from the FJsonObject
					//UE_LOG(LogTemp, Warning, TEXT("Name: %s, Age: %d"), *MyConvertedData.Name, MyConvertedData.Age);

					for (FAssemblyBrick brick : bricklist.bricks)
					{ 
						UE_LOG(LogTemp, Warning, TEXT("Layer: %d"), brick.layerInd );
						UE_LOG(LogTemp, Warning, TEXT("ShortName: %s"), *brick.shortName);
						UE_LOG(LogTemp, Warning, TEXT("Pos: %f %f %f"), brick.position.X, brick.position.Y, brick.position.Z);
						UE_LOG(LogTemp, Warning, TEXT("Rot: %f %f %f %f"), brick.rotation.X, brick.rotation.Y, brick.rotation.Z, brick.rotation.W);
						UE_LOG(LogTemp, Warning, TEXT("Material: %s"), *brick.materialPathName);

					}

				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Failed to convert FJsonObject to FAssemblyBrickList."));
				}

				//UE_LOG(LogTemp, Warning, TEXT("Name: %s, Version: %s"), *Name, *Version));
			}
			else
			{
				// Error deserializing the JSON string
				UE_LOG(LogTemp, Error, TEXT("Failed to deserialize JSON string."));
			}

		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("FileManipulation: Did not load text from file"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("FileManipulation: ERROR: Can not read the file because it was not found."));
		UE_LOG(LogTemp, Warning, TEXT("FileManipulation: Expected file location: %s"), *file);
	}
}

void UAssembly::SaveAssembly(const int Value)
{
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


			//AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(Data->ActorBlueprint, Transform);

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
			UE_LOG(LogTemp, Warning, TEXT("FJsonSerializer: ERROR: Failed to convert JsonObject to string."));
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("FJsonSerializer: SUCCESS\n%s"), *JsonString);

			FString file = FPaths::ProjectConfigDir();
			file.Append(TEXT("Rocket.json"));

			// We will use this FileManager to deal with the file.
			IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();

			if (FFileHelper::SaveStringToFile(JsonString, *file))
			{
				UE_LOG(LogTemp, Warning, TEXT("FileManipulation: Succesfuly written: \"%s\" to the text file"), *JsonString);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("FileManipulation: Failed to write FString to file."));
			}

		}
	}

	// Clear assembly and try to reload.
	TArray<USceneComponent*> ChildSceneComponents;
	GetChildrenComponents(false, ChildSceneComponents);
	for (USceneComponent* Child : ChildSceneComponents)
	{
		if (Child)
		{
			UE_LOG(LogTemp, Warning, TEXT("  - %s (Class: %s)"), *Child->GetName(), *Child->GetClass()->GetName());
			Child->GetOwner()->Destroy(true, true); //  Destroy();
		}
	}

	LoadAssembly();
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
