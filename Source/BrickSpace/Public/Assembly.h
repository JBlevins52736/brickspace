// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
class UBrick;

#include "Assembly.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BRICKSPACE_API UAssembly : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAssembly();

	static void LoadAssembly(FString fname);
	static void SaveAssembly(FString fname);

	static bool PlayMode();
	static bool TryAddBrick(UBrick* brick);
	static bool TryRemoveBrick(UBrick* brick);


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	int AddLayer(int layerind, UBrick*brick);

	static UAssembly* _Instance;
	static TArray<UBrick*> bricks;	
};
