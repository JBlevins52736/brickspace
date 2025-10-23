// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "BrickSpacePawn.h"
#include "WallMover.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BRICKSPACE_API UWallMover : public USceneComponent
{
	GENERATED_BODY()

public:	

	UWallMover();

	UFUNCTION(BlueprintCallable, Category = "Wall Movement")
	void SetMovementTarget(float LeverAngle);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button")
	UStaticMeshComponent* FloorPlate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button")
	UStaticMeshComponent* SkyPlate;


protected:

	virtual void BeginPlay() override;

public:	

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(Replicated) 
	float CurrentPercentage = 0.0f;
private:



	UPROPERTY(EditAnywhere, Category = "Wall Control")
	float LoweredZOffset = -90.0f; 

	UPROPERTY(EditAnywhere, Category = "Wall Control")
	float RaisedZOffset = 90.0f;


	UPROPERTY(EditAnywhere, Category = "Wall Control")
	float MovementInterpSpeed = .3f;

	float CurrentLeverAngle = 0.0f;
	FVector InitialRelativeLocation;

	UPROPERTY(Replicated)
	FVector TargetRelativeLocation;

	UPROPERTY(EditAnywhere, Category = "Ground Effect")
	float GroundMaxScaleY = 1.2f;


	UPROPERTY(EditAnywhere, Category = "Ground Effect")
	float GroundScaleInterpSpeed = 10.0f;

	bool bIsInitialized = false;
	FVector LoweredRelativeLocation;
	bool bWallIsClear = false;
	FVector RaisedRelativeLocation;


};
