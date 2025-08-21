// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VodgetButton.h"
#include "UIMainMenu.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BRICKSPACE_API UUIMainMenu : public UVodgetButton
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void changeScene(bool F);
	UFUNCTION(BlueprintCallable)
	void SpawnMenu();

	UPROPERTY(BlueprintReadWrite)
	USceneComponent* loc; 

	UPROPERTY(BlueprintReadWrite)
	AActor* ActorUi;
};
