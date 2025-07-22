// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Brick.h"

#include "Assembly.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BRICKSPACE_API UAssembly : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAssembly();

	void LoadAssembly(FString fname);
	void SaveAssembly(FString fname);

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
