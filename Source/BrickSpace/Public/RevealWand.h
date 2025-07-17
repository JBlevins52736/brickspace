// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tool.h"
#include "RevealWand.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BRICKSPACE_API URevealWand : public UTool
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "VAR")
	UMaterialInterface* revealMaterial;

protected:
	virtual void BeginPlay() override;
	virtual void ChangeBrickTouched() override;

};
