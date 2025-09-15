// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Grabber.h"
#include "AlignWorld.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BRICKSPACE_API UAlignWorld : public UGrabber
{
	GENERATED_BODY()
	void ForePinch(USelector* selector, bool state) override;

};
