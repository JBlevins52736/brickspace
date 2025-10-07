// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "WorldGrabber.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BRICKSPACE_API UWorldGrabber : public USceneComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UWorldGrabber();

	UFUNCTION(Server, Reliable)
	void Server_Move(UWorldGrabber* TargetActor, FTransform transform);

	virtual void SetWorldToMeters(float worldScale);

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VAR", meta = (AllowPrivateAccess = true))
	USceneComponent* leftHand;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VAR", meta = (AllowPrivateAccess = true))
	USceneComponent* rightHand;

	virtual void ActiveChanged();

	void UpdateCursors();
	virtual void SetLocalCursor();
	FTransform childsrt;
	FTransform cursorsrt;
	bool dollyMode = true;
	bool scaleMode = false;
	bool activeMode = false;

	float currWorldToMeters = 100.0;
	float initialWorldToMeters = 100.0;

	virtual void GrabChanged();
	void StartWorldScaling(FVector lhand, FVector rhand);
	void ChangeWorldScaling(FVector lhand, FVector rhand);
public:

	// Bluprint mappable world grabber input functions
	UFUNCTION(BlueprintCallable, Category = "VAR", meta = (AllowPrivateAccess = true))
	void LWorldGrab(const bool Value);

	UFUNCTION(BlueprintCallable, Category = "VAR", meta = (AllowPrivateAccess = true))
	void RWorldGrab(const bool Value);

	UFUNCTION(BlueprintCallable, Category = "VAR", meta = (AllowPrivateAccess = true))
	void DollyToggle(const bool Value);

	UFUNCTION(BlueprintCallable, Category = "VAR", meta = (AllowPrivateAccess = true))
	void ScaleToggle(const bool Value);

	UFUNCTION(BlueprintCallable, Category = "VAR", meta = (AllowPrivateAccess = true))
	void ActivateToggle(const bool Value);

	void HandTrackActivateToggle(const bool value);
private:
	bool leftGrabbing = false;
	bool rightGrabbing = false;

	float initialBimanualHandDist;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


};
