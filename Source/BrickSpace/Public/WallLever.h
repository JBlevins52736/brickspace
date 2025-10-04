// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Vodget.h"
#include "WallLever.generated.h"




class UStaticMeshComponent;
class USelector;

//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCrankTurn);

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
	void UpdateLeverFromSelector(class USelector* cursor);




	// Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Handle")
	USceneComponent* HandleSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lever")
	USceneComponent* LeverArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lever Target")
	USceneComponent* WallToMove;

	FVector WallStartLocation;

	// Crank settings
	int MaxCrankSteps = 50;       // total units to move
	int CurrentCrankStep = 0;    // how many steps currently applied
	float CrankUnit = 5.f;       // distance per step
	float CrankSpeed = 5.f;      // units per second
	// Internals
	bool bCrankingForward = false; // moving down
	bool bCrankingBackward = false; // moving up
	float CrankTargetZ = 0.f;
protected:

	virtual void BeginPlay() override;
	USelector* grabbingSelector = nullptr;
	float initialGrabZ = 0.0f; // initial local cursor position at grab
	FRotator initialRotation;

	UPROPERTY(EditAnywhere, Category = "Lever Settings")
	float MinPitch = -90.0f;

	UPROPERTY(EditAnywhere, Category = "Lever Settings")
	float MaxPitch = 45.0f;

	UPROPERTY(EditAnywhere, Category = "Lever Settings")
	float Sensitivity = 1.0f;

private:



	void UpdateLeverRotation();
	void BroadcastLeverValue();
		
	
};
