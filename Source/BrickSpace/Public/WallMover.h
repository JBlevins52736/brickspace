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

	UWallMover();

	UFUNCTION(BlueprintCallable, Category = "Wall Movement")
	void SetMovementTarget(float LeverAngle);


protected:

	virtual void BeginPlay() override;

public:	

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
private:

	float CurrentPercentage = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Wall Control")
	float LoweredZOffset = -90.0f; 

	UPROPERTY(EditAnywhere, Category = "Wall Control")
	float RaisedZOffset = 90.0f;


	UPROPERTY(EditAnywhere, Category = "Wall Control")
	float MovementInterpSpeed = .5f;

	float CurrentLeverAngle = 0.0f;
	FVector InitialRelativeLocation;

	UPROPERTY(Replicated)
	FVector TargetRelativeLocation;

	bool bIsInitialized = false;
	FVector LoweredRelativeLocation;
	
	FVector RaisedRelativeLocation;
};
