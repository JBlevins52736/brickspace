// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WorldGrabber.h"
#include "WorldGrabberReplicated.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BRICKSPACE_API UWorldGrabberReplicated : public UWorldGrabber
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;


	void OnRep_WorldScale(float worldScale);

protected:
	virtual void SetWorldToMeters(float val) override;
};
