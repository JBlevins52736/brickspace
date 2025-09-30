// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Vodget.h"

#include "AlignWorld.generated.h"

// Define a float variable delegate that multiple blueprint clients can register callbacks for when the value changes.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPosChangeDelegate, FVector, anchorPos);
/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BRICKSPACE_API UAlignWorld : public UVodget
{
	GENERATED_BODY()

public:

	void ForePinch(USelector* selector, bool state) override;

	UPROPERTY(BlueprintAssignable, Category = "VAR")
	FPosChangeDelegate AlignDelegate;

private:

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	USelector* grabbingSelector = nullptr;
};
