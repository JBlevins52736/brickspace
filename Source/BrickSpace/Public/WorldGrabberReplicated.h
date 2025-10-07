// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WorldGrabber.h"
#include "WorldGrabberReplicated.generated.h"

USTRUCT()
struct FCalibration {
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FVector position;

	UPROPERTY()
	FQuat rotation;

	UPROPERTY()
	float worldToMeters;
};

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BRICKSPACE_API UWorldGrabberReplicated : public UWorldGrabber
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	virtual void ActiveChanged() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VAR", meta = (AllowPrivateAccess = true))
	USceneComponent* leftHandReplicated;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VAR", meta = (AllowPrivateAccess = true))
	USceneComponent* rightHandReplicated;

	void OnRep_WorldScale(float worldScale);

protected:
	virtual void SetWorldToMeters(float val) override;
	void LoadCalibration();
	void SaveCalibration();
};
