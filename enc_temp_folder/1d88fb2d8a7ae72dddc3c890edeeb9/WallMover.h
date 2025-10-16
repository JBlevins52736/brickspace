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

	/** Reference to the Static Mesh Component (the plane) to scale for the ground opening effect. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button")
	UStaticMeshComponent* FloorPlate;
protected:

	virtual void BeginPlay() override;

public:	

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;



private:

	UPROPERTY(Replicated) 
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

	

	/** The maximum Y-scale the plane should reach when the wall is lowered (e.g., 1.2). */
	UPROPERTY(EditAnywhere, Category = "Ground Effect")
	float GroundMaxScaleY = 1.2f;

	/** The speed at which the plane's Y-scale interpolates to the target. */
	UPROPERTY(EditAnywhere, Category = "Ground Effect")
	float GroundScaleInterpSpeed = 5.0f;

	bool bIsInitialized = false;
	FVector LoweredRelativeLocation;
	bool bWallIsClear = false;
	FVector RaisedRelativeLocation;


};
