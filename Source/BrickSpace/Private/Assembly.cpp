// Fill out your copyright notice in the Description page of Project Settings.


#include "Assembly.h"
#include "Brick.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "Misc/FileHelper.h"
#include "CoreMinimal.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include <Kismet/GameplayStatics.h>

#include "BrickSpacePlayerState.h"
#include "VodgetSpawner.h" // FSpawnableData


// Sets default values for this component's properties
UAssembly::UAssembly()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

// Called when the game starts
void UAssembly::BeginPlay()
{
	Super::BeginPlay();

	ENetMode CurrentNetMode = GetNetMode();
	if (CurrentNetMode != NM_ListenServer)
	{
		// This instance is not the server
		return;
	}

	APlayerState* PlayerStateAtIndex0 = UGameplayStatics::GetPlayerState(GetWorld(), 0);
	playerState = Cast<ABrickSpacePlayerState>(PlayerStateAtIndex0);
	if (!playerState)
		return;

	InitMaterialMap();
	InitAssemblyArray();

	// Find the groundplate bricks before snapped bricks might be added to the hierarchy.
	GetOwner()->GetComponents<UBrick>(groundPlateBricks);

	LoadAssembly();
}

void UAssembly::InitMaterialMap()
{
	if (!MaterialTable) return;

	TArray<FName> CachedRowNames = MaterialTable->GetRowNames();
	for (const FName& RowName : CachedRowNames)
	{
		const FMaterialData* Data = MaterialTable->FindRow<FMaterialData>(RowName, TEXT("InitMaterialMaps"));
		UE_LOG(LogTemp, Log, TEXT("Color: %s"), *(RowName.ToString()));
		if (Data != nullptr)
		{
			solidToReveal.Add(Data->SolidMaterial, Data->RevealMaterial);
		}
	}
	//UE_LOG(LogTemp, Log, TEXT("VodgetSpawner: Cached %d short names"), ShortNameToRowNameMap.Num());
}

void UAssembly::InitAssemblyArray()
{
	assemblyTableInd = -1;

	// Load from local Rocket.json file if it exists
	{
		// The Rocket.json file may be created either running in link mode on the PC or in a headset build.
		// When running in link mode the assembly is saved directly in the projects Config folder.
		// When running in a build the saved assembly can be accessed by allowing the Quest3 to be mounted as a USB drive and  
		// can be downloaded from This PC\Quest 3\Internal shared storage\Android\data\com.mixeduprealities.BrickSpace\files\UnrealGame\BrickSpace\BrickSpace\Config
		// The Rocket.json that exists in the projects Config folder at the time of a build becomes the project default that is 
		// loaded during BeginPlay().
		// There are two developer buttons to be used to design alternate assemblies. CreateAssembly and SaveAssembly.
		// CreateAssembly removes all bricks attached to the ground plate and initiates free building mode.
		// SaveAssembly saves all bricks attached to the ground plate overwriting Rocket.json, exits free building mode, 
		// and reloads Rocket.json initiating game play mode.
		// 
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
					FAssemblyBrickList bricklist;
					if (FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &bricklist, 0, 0))
					{
						UE_LOG(LogTemp, Error, TEXT("Converted Local FJsonObject to FAssemblyBrickList."));
						assemblyTable.Add(bricklist);
					}
					else
					{
						UE_LOG(LogTemp, Error, TEXT("Failed to convert FJsonObject to FAssemblyBrickList."));
					}
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

	for (const FString& JsonString : JSONTable)
	{
		// Create a TSharedPtr to hold the FJsonObject
		TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());

		// Create a TJsonReader to read the JsonString
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

		// Attempt to deserialize the JSON string into the FJsonObject
		if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
		{
			// Successfully deserialized the JSON string into JsonObject
			FAssemblyBrickList bricklist;
			if (FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &bricklist, 0, 0))
			{
				UE_LOG(LogTemp, Error, TEXT("Converted FJsonObject to FAssemblyBrickList."));
				assemblyTable.Add(bricklist);
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
	//UE_LOG(LogTemp, Log, TEXT("VodgetSpawner: Cached %d short names"), ShortNameToRowNameMap.Num());
}

UBrick* UAssembly::SpawnBrick(const FAssemblyBrick& brick)
{
	if (SpawnDataTable == nullptr)
		return nullptr;

	FName RowName(*brick.shortName);
	FSpawnableData* Data = SpawnDataTable->FindRow<FSpawnableData>(RowName, TEXT("SpawnBrick"));
	if (Data == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("Table ROW NOT FOUND"));
		return nullptr;
	}
	UE_LOG(LogTemp, Warning, TEXT("TABLE Data found: Spawning"));

	// When spawned from default blueprint pos, rot, solidMatchMaterial, and brickMaterial need to be changed.
	AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(Data->ActorBlueprint);
	UBrick* SpawnedBrick = SpawnedActor->FindComponentByClass<UBrick>();

	FTransform Transform(brick.rotation, brick.position);
	SpawnedActor->GetRootComponent()->SetRelativeTransform(Transform);

	SpawnedBrick->groundplateActor = this->GetOwner();
	//SpawnedActor->GetRootComponent()->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);

	// Reveal material is a translucent version of the solidMatchMaterial
	UMaterialInterface* RevealMaterial = *(solidToReveal.Find(brick.material));

	SpawnedBrick->solidMatchMaterial = brick.material;
	SpawnedBrick->isSolid = false;
	SpawnedBrick->isGrabbable = false;
	SpawnedBrick->brickMaterial = RevealMaterial;
	SpawnedBrick->OnRep_Material();
	SpawnedBrick->OnRep_Grabbable();
	SpawnedBrick->OnRep_Parent();
	
	return SpawnedBrick;
}

bool GameModeActive = true;
void UAssembly::LoadAssembly()
{
	if (assemblyTable.Num() == 0)
		return;

	GameModeActive = true;
	assemblyTableInd = (++assemblyTableInd) % assemblyTable.Num();

	currLayer = -1;
	if (!LoadNextLayer()) {
		UE_LOG(LogTemp, Warning, TEXT("bricklist initialzed but empty"));
	}
}

bool UAssembly::TryAdvanceLayer()
{
	ENetMode CurrentNetMode = GetNetMode();
	if (CurrentNetMode != NM_ListenServer)
	{
		// This instance is not the server
		return false;
	}

	for (int i = 0; i < layerBricks.size(); i++)
	{
		UBrick* brick = layerBricks[i];
		if (!brick->IsSolved())
			return false;
	}
	if (!LoadNextLayer()) {
		// Take off!
		startPos = GetComponentLocation();
		PrimaryComponentTick.SetTickFunctionEnable(true);
	}
	return true;
}

bool UAssembly::LoadNextLayer()
{
	++currLayer;
	layerBricks.clear();
	FAssemblyBrickList bricklist = assemblyTable[assemblyTableInd];
	for (FAssemblyBrick brick : bricklist.bricks)
	{
		if (brick.layerInd == currLayer) {
			//UE_LOG(LogTemp, Warning, TEXT("Layer: %d"), brick.layerInd);
			//UE_LOG(LogTemp, Warning, TEXT("ShortName: %s"), *brick.shortName);
			//UE_LOG(LogTemp, Warning, TEXT("Pos: %f %f %f"), brick.position.X, brick.position.Y, brick.position.Z);
			//UE_LOG(LogTemp, Warning, TEXT("Rot: %f %f %f %f"), brick.rotation.X, brick.rotation.Y, brick.rotation.Z, brick.rotation.W);
			//UE_LOG(LogTemp, Warning, TEXT("Material: %s"), *brick.material->GetPathName());

			UBrick* spawnedBrick = SpawnBrick(brick);
			if (spawnedBrick != nullptr)
				layerBricks.push_back(spawnedBrick);
		}
	}
	return (layerBricks.size() > 0);
}

void UAssembly::ClearAssembly()
{
	// Clear all children attached to current assembly.
	TArray<USceneComponent*> ChildSceneComponents;
	GetChildrenComponents(false, ChildSceneComponents);
	for (USceneComponent* Child : ChildSceneComponents)
	{
		if (Child)
		{
			UE_LOG(LogTemp, Warning, TEXT("  - %s (Class: %s)"), *Child->GetName(), *Child->GetClass()->GetName());
			// Note: We must delete the actor blueprint that is the owner of each child brick scenecomponent.
			Child->GetOwner()->Destroy(true, true); //  Destroy();
		}
	}

	// Remove dead pointers
	for (int i = 0; i < groundPlateBricks.Num(); ++i)
		for (int s = 0; s < groundPlateBricks[i]->studs.size(); s++)
			groundPlateBricks[i]->studs[s]->snappedTo = nullptr;
}

void UAssembly::CreateAssembly(const int Value)
{
	if (!GameModeActive)
		return;
	GameModeActive = false;

	ClearAssembly();
}

void UAssembly::SaveAssembly(const int Value)
{
	// Assembly cannot be saved when game mode is active. Developer must call CreateAssembly first.
	if (GameModeActive)
		return;


	// Populate currLayerBricks with ground plate bricks as first (untracked) layer.
	std::vector<UBrick*> currLayerBricks;
	for (int i = 0; i < groundPlateBricks.Num(); ++i)
		currLayerBricks.push_back(groundPlateBricks[i]);

	if (currLayerBricks.size() > 0) {

		int layer = 0;
		FAssemblyBrickList brickList;
		while (currLayerBricks.size() > 0)
		{
			// Change all connected bricks parent to the assembly sceneComponent.
			// Returns only the connected bricks that were reparented as the next assembly layer.
			std::vector<UBrick*> reparentedBricks;
			for (int i = 0; i < currLayerBricks.size(); ++i)
				currLayerBricks[i]->ReparentConnectedBricks(this, reparentedBricks);

			if (SpawnDataTable != nullptr)
			{
				// Add all reparented bricks to FAssemblyBrick list to be saved/serialized as JSON.
				for (int i = 0; i < reparentedBricks.size(); ++i) {

					brickList.bricks.Add(FAssemblyBrick{
						layer,
						reparentedBricks[i]->shortName,
						reparentedBricks[i]->GetLocation(),
						reparentedBricks[i]->GetQuat(),
						reparentedBricks[i]->GetMaterial() });
				}
			}

			currLayerBricks = reparentedBricks;
			reparentedBricks.clear();
			++layer;
		};

		// Note: Unless Rabbit.json text is added to the assemblers JSONTable property
		// in the ground plate blueprint, it will not be part of the next build.
		assemblyTable.Add(brickList);
		assemblyTableInd = assemblyTable.Num() - 2;

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

	}

	// Clear assembly and reload to restart in game play mode.
	ClearAssembly();
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

void UAssembly::OnRep_RocketPos()
{
	SetWorldLocation(rocketPos);
}

void UAssembly::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	// Fly upward at velocityCmPerSec until well over the ceiling (about 10m or 1000cm)
	FVector pos = GetComponentLocation();
	if (pos.Z > 500.0) {
		PrimaryComponentTick.SetTickFunctionEnable(false);
		ClearAssembly();
		SetWorldLocation(startPos);
		LoadAssembly();
		return;
	}

	pos += FVector::UpVector * velocityCmPerSec * DeltaTime;

	rocketPos = pos;
	OnRep_RocketPos();

	//SetWorldLocation(pos);
}
