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

void USnapBase::TrySnap(USnapBase* othSnap)
{
	// If already snappedTo then ApplySnap has already been solved.
	if (snappedTo == nullptr)
	{
		FVector pos = this->GetComponentLocation();
		FVector othpos = othSnap->GetComponentLocation();
		float distSq = FVector::DistSquared(pos, othpos);
		// Search for the closest othSnap withing snap distance.
		if (distSq < SnapDistSq && !(trySnappedTo != nullptr && distSq > trySnapDistSq)) {
			// Found a new, or improved, snap candidate.
			trySnappedTo = othSnap;
			trySnapDistSq = distSq;
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
	if (trySnappedTo != nullptr) {
		snappedTo = trySnappedTo;
		snappedTo->snappedTo = this;
		trySnappedTo = nullptr;
	}

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
		FVector ds = pivot.GetLocation() - pos;
		clientComponent->AddWorldOffset(ds);
	}
	else if (snapcnt == 1) {
		// The first snap causes the brick to translate and rotate about a pivot altering subsequent snaps distance criteria.
		// Any subsequent snap must be to locations exactly on the LUnit grid to avoid diagonal snapping.
		// If distance between snapto location and pivot is not 78 then reject it as a false diagonal snap.
		// Inter-stud spacing is 78 in modelling space but 19.5 when scaled 25% in blueprint.
		FVector toDir = snappedTo->GetComponentLocation() - pivot.GetLocation();
		float len = toDir.Length();
		if ( len < 19.0 || len > 20.0 ) {
			// Diagonal or irregular snap detected.
			snappedTo->snappedTo = nullptr;
			snappedTo = nullptr;
			//UE_LOG(LogTemp, Warning, TEXT("Diagonal snap detected:%f not 19.5"), len );
			return false;
		}
		toDir /= len;

		// Otherwise we rotate to a rigid alignment
		FVector fromDir = (this->GetComponentLocation() - pivot.GetLocation()).GetUnsafeNormal();
		FQuat dq = FQuat::FindBetweenNormals(fromDir, toDir);

		FVector PivotToComponent = clientComponent->GetComponentLocation() - pivot.GetLocation();
		FVector RotatedPivotToComponent = dq.RotateVector(PivotToComponent);
		FVector worldpos = pivot.GetLocation() + RotatedPivotToComponent;
		clientComponent->AddWorldRotation(dq);
		clientComponent->SetWorldLocation(worldpos);

	}
	else {
		// We only need 2 snaps to make a rigid joint, so theres no need to continue solving.
	}

	return true;
}
