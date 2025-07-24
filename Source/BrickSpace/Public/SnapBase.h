// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
class UBrick;

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
	bool IsSnapped() { return (snappedTo != nullptr); }
	USnapBase* snappedTo = nullptr;

	// Note: Storing brick is a load time efficiency to improve walking through snap graph recursively.
	UBrick* brick = nullptr;	// The UBrick that this USnapBase (Stud/Tube) is attached to.

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	// Thin brick distance is 32, if two thin bricks are snapped on top of each other the top stud would be 64 from the bottom most tube.
	// We cannot allow the top stud to snap to the bottom most tube so we set SnapDist to 60 even though the inter-stud spacing is 78.
	// With SnapDist at 60, we double it for unsnap distance to avoid hysterisis.
	// Note: Dimensions above are in the bricks current modelled state and don't account for scaled down by 25% in blueprints.
	// It would be best if we removed blueprint scaling by re-imported all bricks at the correct size.
	const float SnapDistSq = 225.0;	// 60.0f squared == 3600  at 25% 15 squared == 225.0
	const float UnsnapDistSq = 900.0; // 120.0f squared == 14,400 at 25% 30.0 squared == 900.0
	float trySnapDistSq;
	USnapBase* trySnappedTo = nullptr;
};
