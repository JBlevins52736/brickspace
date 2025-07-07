// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "SnapBase.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BRICKSPACE_API USnapBase : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USnapBase();

	void TryBreakSnap();
	void TrySnap(USnapBase*);
	bool ApplySnap(USceneComponent* clientComponent, FTransform &pivot, int snapcnt);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	const float SnapDist = 32.0f;
	const float UnsnapDist = 64.0f;
	USnapBase* snappedTo = nullptr;
};
