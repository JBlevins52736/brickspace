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
	// Thin brick distance is 32, if two thin bricks are snapped on top of each other the top stud would be 64 from the bottom most tube.
	// We cannot allow the top stud to snap to the bottom most tube so we set SnapDist to 60 even though the inter-stud spacing is 78.
	const float SnapDist = 60.0f;  
	const float UnsnapDist = 120.0f;
	USnapBase* snappedTo = nullptr;
};
