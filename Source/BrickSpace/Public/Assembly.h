// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Brick.h"

#include "Assembly.generated.h"

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
	FString materialPathName;
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
	void SaveAssembly(const int Value);

	bool PlayMode();
	bool TryAddBrick(UBrick* brick);
	bool TryRemoveBrick(UBrick* brick);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	TArray<UBrick*> groundPlateBricks;
	std::vector<UBrick*> assembledBricks;
};
