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
	virtual void ForePinch(USelector* selector, bool state) override;

	UPROPERTY(EditAnywhere, Category = "VAR")
	UMaterialInterface* brushMaterial;

protected:
	virtual void BeginPlay() override;
	virtual void ChangeBrickTouched() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
