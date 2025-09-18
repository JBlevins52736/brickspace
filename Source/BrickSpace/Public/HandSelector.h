// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Selector.h"
#include "HandSelector.generated.h"

/**
 *
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BRICKSPACE_API UHandSelector : public USelector
{
	GENERATED_BODY()

public:

	UHandSelector();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VAR")
	USceneComponent* centerEye = nullptr;

	UPROPERTY(ReplicatedUsing = OnRep_MeshPosUpdate)
	FVector handPos;

	UFUNCTION()
	virtual void OnRep_MeshPosUpdate();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void SetFilter(uint16 filter) override;

protected:
	// Called when the game starts
	virtual void SetCursor() override;

	virtual void GrabFocus(bool val) override;

private:
	UVodget* DoRaycast();

	float ratioHitEyeOverHandEye = 0.0;

	// The hit result gets populated by the line trace
	FHitResult Hit;
	
};
