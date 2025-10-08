// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Vodget.h"
#include "WallLever.generated.h"

class UStaticMeshComponent;
class USelector;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLeverMoved, float, NormalizedRotation);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BRICKSPACE_API UWallLever : public UVodget
{
	GENERATED_BODY()

public:	

	UWallLever();

	virtual void Focus(USelector* cursor, bool state) override;
	
	virtual void ForePinch(USelector* cursor, bool state) override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void UpdateLeverFromSelector(class USelector* cursor);


	UPROPERTY(BlueprintAssignable, Category = "Lever Events")
	FOnLeverMoved OnLeverMoved;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Handle")
	USceneComponent* HandleSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lever")
	USceneComponent* LeverArm;

protected:

	virtual void BeginPlay() override;
	USelector* grabbingSelector = nullptr;
	
	FVector grabvec;

	UPROPERTY(EditAnywhere, Category = "Lever Settings")
	float MinPitch = -75.0f;

	UPROPERTY(EditAnywhere, Category = "Lever Settings")
	float MaxPitch = 75.0f;

	UPROPERTY(EditAnywhere, Category = "Lever Settings")
	float Sensitivity = 1.0f;

private:

		
	
};
