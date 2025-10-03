// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Vodget.h"
#include "WallLever.generated.h"




class UStaticMeshComponent;
class USelector;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLeverMoved, float, LeverValue);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BRICKSPACE_API UWallLever : public UVodget
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWallLever();
	virtual void Focus(USelector* cursor, bool state) override;
	virtual void ForePinch(USelector* cursor, bool state) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;



	// Event broadcast when lever moves (0.0–1.0)
	UPROPERTY(BlueprintAssignable, Category = "Lever Events")
	FOnLeverMoved LeverDelegate;

	// Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Handle")
	USceneComponent* HandleSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lever")
	USceneComponent* LeverArm;
protected:

	USelector* grabbingSelector = nullptr;
	float initialGrabX = 0.0f; // initial local cursor position at grab
	FRotator initialRotation;

	UPROPERTY(EditAnywhere, Category = "Lever Settings")
	float MinPitch = -45.0f;

	UPROPERTY(EditAnywhere, Category = "Lever Settings")
	float MaxPitch = 45.0f;

	UPROPERTY(EditAnywhere, Category = "Lever Settings")
	float Sensitivity = 1.0f;

private:



	void UpdateLeverRotation();
	void BroadcastLeverValue();
		
	
};
