// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#define WORLDTOMETERS_SCALING

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WorldGrabber.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BRICKSPACE_API UWorldGrabber : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UWorldGrabber();

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VAR", meta = (AllowPrivateAccess = true))
	USceneComponent* leftHand;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VAR", meta = (AllowPrivateAccess = true))
	USceneComponent* rightHand;

private:

	// Bluprint mappable world grabber input functions
	UFUNCTION(BlueprintCallable, Category = "VAR", meta = (AllowPrivateAccess = true))
	void LWorldGrab(const bool Value);

	UFUNCTION(BlueprintCallable, Category = "VAR", meta = (AllowPrivateAccess = true))
	void RWorldGrab(const bool Value);

	UFUNCTION(BlueprintCallable, Category = "VAR", meta = (AllowPrivateAccess = true))
	void DollyToggle(const bool Value);

	UFUNCTION(BlueprintCallable, Category = "VAR", meta = (AllowPrivateAccess = true))
	void ScaleToggle(const bool Value);

	void SetLocalCursor();

	bool leftGrabbing = false;
	bool rightGrabbing = false;

	FTransform childsrt;
	FTransform cursorsrt;
	float initialBimanualHandDist;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void GrabChanged();

	bool dollyMode = true;
	bool scaleMode = false;
#ifdef WORLDTOMETERS_SCALING
	float initialWorldToMeters = 100.0;
	float currWorldToMeters = 100.0;
#endif
};
