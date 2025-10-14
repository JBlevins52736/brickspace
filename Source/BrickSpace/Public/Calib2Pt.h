// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WorldGrabberReplicated.h"
#include "Calib2Pt.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BRICKSPACE_API UCalib2Pt : public UWorldGrabberReplicated
{
	GENERATED_BODY()
	
public:
	virtual void ActiveChanged() override;

	UFUNCTION(BlueprintCallable, Category = "VAR")
	void UpdatePosAnchor(FVector pos);

	UFUNCTION(BlueprintCallable, Category = "VAR")
	void UpdateRotAnchor(FVector pos);

protected:
	virtual void SetLocalCursor() override;
	
private:

	bool posAnchorSet = false;
	bool rotAnchorSet = false;
	FVector posAnchor;
	FVector rotAnchor;

	FVector posAnchorInit;
	FVector posAnchorLocal;

	float initWorldToMeters;
};
