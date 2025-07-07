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
		if (FVector::DistSquared(pos, othpos) < SnapDist)
			snappedTo = tube;
	}
}

void USnapBase::TryBreakSnap()
{
	if (snappedTo != nullptr)
	{
		FVector pos = this->GetComponentLocation();
		FVector othpos = snappedTo->GetComponentLocation();
		if (FVector::DistSquared(pos, othpos) > UnsnapDist)
			snappedTo = nullptr;
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

		// Setting world rotation will (should) shift this components location.
		clientComponent->SetWorldRotation(snappedTo->GetComponentQuat());

		// Translate by the new pos to pivot delta vector.
		FVector pos = this->GetComponentLocation();
		FVector ds = pos - pivot.GetLocation();
		clientComponent->AddWorldOffset(ds);
	}
	else if (snapcnt == 1) {
		// Rotate to rigid only if distance between othpos and first snap pos is 78

	}

	return true;
}
