// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Vodget.h"

#include "HandColorChanger.generated.h"

/**
 * Note: Component not currently used
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BRICKSPACE_API UHandColorChanger : public UVodget
{
	GENERATED_BODY()
	virtual void Focus(USelector* cursor, bool state) override;

	UPROPERTY(EditAnywhere, Category = "VAR")
	UMaterialInterface* brushMaterial;
};
