// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "VodgetButton.h"
#include "Camera/CameraComponent.h"
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

	UFUNCTION(BlueprintCallable, Category = "UI Location")
	void UpdateUIScreenComp();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configs")
	float distanceFromFace = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configs")
	float UIFovUpdate = 0.10f;
	
	void BeginPlay() override;

	
private:

	UCameraComponent* camera = nullptr;
};
