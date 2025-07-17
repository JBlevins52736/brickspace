// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tool.h"
#include "Paintbrush.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BRICKSPACE_API UPaintbrush : public UTool
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "VAR")
	UMaterialInterface* brushMaterial;

protected:
	virtual void BeginPlay() override;
	virtual void ChangeBrickTouched() override;

};
