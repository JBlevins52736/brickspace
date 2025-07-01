// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Vodget.h"
#include "Grabber.generated.h"

/**
 *
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BRICKSPACE_API UGrabber : public UVodget
{
	GENERATED_BODY()

public:
	virtual void Focus(USelector* selector, bool state) override;
	virtual void ForePinch(USelector* selector, bool state) override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:

	USelector* grabbingSelector = nullptr;
	FTransform childsrt;
};
