// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
class USelector;

#include "Vodget.generated.h"

UCLASS(Abstract, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BRICKSPACE_API UVodget : public USceneComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UVodget();

	virtual void Focus(USelector* cursor, bool state);
	virtual void ForePinch(USelector* cursor, bool state);
	virtual void MiddlePinch(USelector* cursor, bool state);
	virtual void Grip(USelector* cursor, bool state);

	UPROPERTY(EditAnywhere, meta = (Bitmask), Category = "VAR")
	uint16 selectionFilter = 0xFF;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VAR", meta = (AllowPrivateAccess = true))
	USceneComponent* clientComponent = nullptr;
};
