// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Brick.h"
#include "Engine/DataTable.h"

#include "Assembly.generated.h"

UENUM(BlueprintType) // Allows this enum to be used in Blueprints
enum class EColorState : uint8 // E prefix is a common Unreal naming convention for enums
{
	Red UMETA(DisplayName = "Red Plastic"), // UMETA allows setting a display name for Blueprint
	Green UMETA(DisplayName = "Green Plastic"),
	Blue UMETA(DisplayName = "Blue Plastic"),
	Yellow UMETA(DisplayName = "Yellow Plastic"),
	Orange UMETA(DisplayName = "Orange Plastic"),
	Grey UMETA(DisplayName = "Grey Plastic"),
	Black UMETA(DisplayName = "Black Plastic")
};

USTRUCT(BlueprintType)
struct FMaterialData : public FTableRowBase
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawnable")
	UMaterialInterface* SolidMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawnable")
	UMaterialInterface* RevealMaterial;
};

USTRUCT()
struct FAssemblyBrick {
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	int layerInd;

	UPROPERTY()
	FString shortName;

	UPROPERTY()
	FVector position;

	UPROPERTY()
	FQuat rotation;

	UPROPERTY()
	UMaterialInterface* material;
};

USTRUCT()
struct FAssemblyBrickList {
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	TArray<FAssemblyBrick> bricks;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BRICKSPACE_API UAssembly : public USceneComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAssembly();

	void LoadAssembly();

	UFUNCTION(BlueprintCallable, Category = "VAR", meta = (AllowPrivateAccess = true))
	void CreateAssembly(const int Value);

	UFUNCTION(BlueprintCallable, Category = "VAR", meta = (AllowPrivateAccess = true))
	void SaveAssembly(const int Value);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
	UDataTable* SpawnDataTable = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
	UDataTable* MaterialTable = nullptr;

	bool PlayMode();
	bool TryAddBrick(UBrick* brick);
	bool TryRemoveBrick(UBrick* brick);
	bool TryAdvanceLayer();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	TArray<UBrick*> groundPlateBricks;

private:
	// HACK: Until table uses shortname as key
	void CacheShortNames();
	TMap<FString, FName> ShortNameToRowNameMap;

	void InitMaterialMap();
	TMap<UMaterialInterface*, UMaterialInterface*> solidToReveal;

	void ClearAssembly();

	UBrick* SpawnBrick(const FAssemblyBrick &brick);
	int currLayer = -1;
	std::vector<UBrick*> layerBricks;
	bool LoadNextLayer();
	FAssemblyBrickList bricklist;
};
