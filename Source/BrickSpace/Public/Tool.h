// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Grabber.h"
#include "Brick.h"

#include "Tool.generated.h"

/**
 * 
 */
UCLASS(Abstract, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BRICKSPACE_API UTool : public UGrabber
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void ChangeBrickTouched();
	
	UBrick *brickTouched = nullptr;

private:
	/** called when something enters the sphere component */
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
