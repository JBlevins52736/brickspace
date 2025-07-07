// Fill out your copyright notice in the Description page of Project Settings.

#include "Brick.h"

void UBrick::BeginPlay()
{
	Super::BeginPlay();

	// Initialze tubes and studs vectors.
	for (int i = 0; i < clientComponent->GetNumChildrenComponents(); i++)
	{
		UTube* tube = Cast<UTube>(clientComponent->GetChildComponent(i));
		if (tube != nullptr) {
			tubes.push_back(tube);
		}
		else {
			UStud* stud = Cast<UStud>(clientComponent->GetChildComponent(i));
			if (stud != nullptr)
				studs.push_back(stud);
		}
	}
}

void UBrick::ForePinch(USelector* selector, bool state)
{
	// Only allow single handed brick grabbing for now.
	if (grabbingSelector != nullptr && grabbingSelector != selector)
		return;

	Super::ForePinch(selector, state);

	// Add overlap with other bricks.
	UStaticMeshComponent* mesh = Cast<UStaticMeshComponent>(clientComponent);
	if (mesh != nullptr)
	{
		if (state) {
			// Add overlap with other bricks.
			mesh->OnComponentBeginOverlap.AddDynamic(this, &UBrick::OnOverlapBegin);
			mesh->OnComponentEndOverlap.AddDynamic(this, &UBrick::OnOverlapEnd);
		}
		else {
			// Remove overlap with other bricks.
			mesh->OnComponentBeginOverlap.RemoveAll(this);
			mesh->OnComponentEndOverlap.RemoveAll(this);
		}
	}
}

void UBrick::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// OverlappedComp is our local clientComponent
	// Search OtherActor for Brick components
	for (int i = 0; i < OtherComp->GetNumChildrenComponents(); i++) {
		UBrick* enteringBrick = Cast<UBrick>(OtherComp->GetChildComponent(i));
		if (enteringBrick != nullptr)
		{
			bricks.push_back(enteringBrick);
			UE_LOG(LogTemp, Warning, TEXT("OnOverlapBegin enteringBrick:%s"), *FString(enteringBrick->GetName()));
			break;
		}
	}
}

void UBrick::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	for (int i = 0; i < OtherComp->GetNumChildrenComponents(); i++) {
		UBrick* exitingBrick = Cast<UBrick>(OtherComp->GetChildComponent(i));
		if (exitingBrick != nullptr)
		{
			bricks.remove(exitingBrick);
			UE_LOG(LogTemp, Warning, TEXT("OnOverlapEnd exitingBrick:%s"), *FString(exitingBrick->GetName()));
			break;
		}
	}
}

void UBrick::UpdateSnaps()
{
	// The brick has been moved to an unsnapped "ghost" position 

	// Try breaking existing snaps.
	{
		for (int tubeind = 0; tubeind < tubes.size(); ++tubeind)
			tubes[tubeind]->TryBreakSnap();
		for (int studind = 0; studind < studs.size(); ++studind)
			studs[studind]->TryBreakSnap();
	}

	// Try making new snaps.
	for (UBrick* brick : bricks) {
		for (int tubeind = 0; tubeind < tubes.size(); ++tubeind) {
			for (int studind = 0; studind < brick->studs.size(); ++studind)
				tubes[tubeind]->TrySnap(brick->studs[studind]);
		}
		for (int studind = 0; studind < studs.size(); ++studind) {
			for (int tubeind = 0; tubeind < brick->tubes.size(); ++tubeind)
				studs[studind]->TrySnap(brick->tubes[tubeind]);
		}
	}

	int snapcnt = 0;
	FTransform pivot;
	for (int tubeind = 0; tubeind < tubes.size(); ++tubeind) {
		if (tubes[tubeind]->ApplySnap(clientComponent, pivot, snapcnt))
			++snapcnt;
	}
}


void UBrick::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	// States: 
	// Seeking: No overlap with other bricks, normal grabbing mode only.
	// Snapping: Normal grabbing mode followed by tube/stud proximity matches.
	// PinSnapped: Unsnap stress test. If fail: Enter Snapping/Seeking mode. Otherwise: Pin rotate followed by tube/stud proximity matches.
	// RigidSnapped: Unsnap stress test only.


	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (brickState == Seeking) {
		return;
	}

	if (brickState == Snapping) {

		UpdateSnaps();

	}

}