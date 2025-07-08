// Fill out your copyright notice in the Description page of Project Settings.


#include "SnapBase.h"

// Sets default values for this component's properties
USnapBase::USnapBase()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

}


// Called when the game starts
void USnapBase::BeginPlay()
{
	Super::BeginPlay();

}

void USnapBase::TrySnap(USnapBase* tube)
{
	if (snappedTo == nullptr)
	{
		FVector pos = this->GetComponentLocation();
		FVector othpos = snappedTo->GetComponentLocation();
		if (FVector::DistSquared(pos, othpos) < SnapDistSq) {
			snappedTo = tube;
			snappedTo->snappedTo = this;
		}
	}
}

void USnapBase::TryBreakSnap()
{
	if (snappedTo != nullptr)
	{
		FVector pos = this->GetComponentLocation();
		FVector othpos = snappedTo->GetComponentLocation();
		if (FVector::DistSquared(pos, othpos) > UnsnapDistSq) {
			snappedTo->snappedTo = nullptr;
			snappedTo = nullptr;
		}
	}
}

bool USnapBase::ApplySnap(USceneComponent* clientComponent, FTransform& pivot, int snapcnt)
{
	if (snappedTo == nullptr)
		return false;

	if (snapcnt == 0) {

		// Set world pivot for pin rotation on subsequent snaps
		pivot.SetLocation(snappedTo->GetComponentLocation());
		pivot.SetRotation(snappedTo->GetComponentQuat());

		// Orient this bricks up vector to the up direction of the brick we are snapping to.
		FVector toDir = snappedTo->GetUpVector();
		FVector fromDir = clientComponent->GetUpVector();
		FQuat dq = FQuat::FindBetweenNormals(fromDir, toDir);
		clientComponent->AddWorldRotation(dq);

		// Translate this bricks location by the delta position between the snaps.
		FVector pos = this->GetComponentLocation();
		FVector ds = pos - pivot.GetLocation();
		clientComponent->AddWorldOffset(ds);
	}
	else if (snapcnt == 1) {
		// If distance between snapto location and pivot is not 78 then reject it as a false diagonal snap.
		FVector toDir = snappedTo->GetComponentLocation() - pivot.GetLocation();
		float len = toDir.Length();
		if (len < 77.0 || len > 79.0)
			return false;
		toDir /= len;

		// Otherwise we rotate to a rigid alignment
		FVector fromDir = (this->GetComponentLocation() - pivot.GetLocation()).GetUnsafeNormal();
		FQuat dq = FQuat::FindBetweenNormals(fromDir, toDir);
		clientComponent->AddWorldRotation(dq);
	}

	return true;
}
