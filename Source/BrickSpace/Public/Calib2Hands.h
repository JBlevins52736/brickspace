// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WorldGrabberReplicated.h"
#include "Calib2Hands.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BRICKSPACE_API UCalib2Hands : public UWorldGrabberReplicated
{
	GENERATED_BODY()
	
public:
	virtual void ActiveChanged() override;

protected:
	virtual void SetLocalCursor() override;
	
private:
	FVector leftPos;
	FVector rightPos;
};
