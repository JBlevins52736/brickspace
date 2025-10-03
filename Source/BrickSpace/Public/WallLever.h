// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Grabber.h"
#include "WallLever.generated.h"



class USphereComponent;
class UStaticMeshComponent;
class USelector;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLeverMoved, float, LeverValue);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BRICKSPACE_API UWallLever : public UGrabber
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWallLever();
	// Handles interaction
	virtual void Focus(USelector* cursor, bool state) override;
	virtual void ForePinch(USelector* cursor, bool state) override;

	// Delegate broadcast when lever value changes (0.0 to 1.0)
	UPROPERTY(BlueprintAssignable, Category = "Lever Events")
	FOnLeverMoved LeverDelegate;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
private:
	// Components
	UPROPERTY()
	USphereComponent* HandleSphere;

	UPROPERTY()
	UStaticMeshComponent* LeverArm;


	// Configuration
	UPROPERTY(EditAnywhere, Category = "Lever Settings")
	float MinPitch = -45.0f;

	UPROPERTY(EditAnywhere, Category = "Lever Settings")
	float MaxPitch = 45.0f;

	UPROPERTY(EditAnywhere, Category = "Lever Settings")
	float Sensitivity = 0.5f;

	// Initial grab state
	float InitialGrabX = 0.0f;
	FRotator InitialRotation;

	// Update logic
	void UpdateLeverRotation();
	void BroadcastLeverValue();
		
	
};
