// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "WallMover.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BRICKSPACE_API UWallMover : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWallMover();

	UFUNCTION(BlueprintCallable, Category = "Wall Movement")
	void SetMovementTarget(float LeverAngle);


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	// --- Configurable Properties ---



	// Target Z-offset for the fully lowered position.
	UPROPERTY(EditAnywhere, Category = "Wall Control")
	float LoweredZOffset = -70.0f; // 5 units down

	// The lever angle at which the wall starts moving DOWN.
	UPROPERTY(EditAnywhere, Category = "Wall Control")
	float LoweredThreshold = -80.0f;

	// The lever angle at which the wall starts moving UP.
	UPROPERTY(EditAnywhere, Category = "Wall Control")
	float RaisedThreshold = 30.0f;

	// The speed of the smooth movement (interpolation speed).
	UPROPERTY(EditAnywhere, Category = "Wall Control")
	float MovementInterpSpeed = 0.5f;

	// --- State Variables ---
	FVector InitialRelativeLocation;
	FVector TargetRelativeLocation;
	bool bIsInitialized = false;
	
};
