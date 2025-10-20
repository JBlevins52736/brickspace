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
	UFUNCTION(BlueprintCallable, Category = "VAR", meta = (AllowPrivateAccess = true))
	void LCalibGesture(const bool Value);

	UFUNCTION(BlueprintCallable, Category = "VAR", meta = (AllowPrivateAccess = true))
	void RCalibGesture(const bool Value);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VAR")
	USceneComponent* dummyL = nullptr;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VAR")
	USceneComponent* dummyR = nullptr;

private:
	void DoCalibration();

	void SetCursorSrt();
	FVector leftPos;
	FVector rightPos;
	FTransform csr;

	bool leftGestureState = false;
	bool rightGestureState = false;
};
