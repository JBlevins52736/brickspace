// Fill out your copyright notice in the Description page of Project Settings.

#include "Brick.h"
#include "Assembly.h"

void UBrick::BeginPlay()
{
	Super::BeginPlay();

	selectionFilter = 0x01; // Only Assembler can grab bricks

	// Initialze tubes and studs vectors.
	for (int i = 0; i < clientComponent->GetNumChildrenComponents(); i++)
	{
		UTube* tube = Cast<UTube>(clientComponent->GetChildComponent(i));
		if (tube != nullptr) {
			tubes.push_back(tube);
			tube->brick = this;
		}
		else {
			UStud* stud = Cast<UStud>(clientComponent->GetChildComponent(i));
			if (stud != nullptr) {
				stud->brick = this;
				studs.push_back(stud);
			}
		}
	}

	//UStaticMeshComponent* mesh = Cast<UStaticMeshComponent>(clientComponent);
	//if (mesh != nullptr)
	//{
	//	// Add overlap with other bricks.
	//	mesh->OnComponentBeginOverlap.AddDynamic(this, &UBrick::OnOverlapBegin);
	//	mesh->OnComponentEndOverlap.AddDynamic(this, &UBrick::OnOverlapEnd);
	//}

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

	if (UAssembly::_Instance->PlayMode() && !state) {

		int snapCnt = 0;
		// On release, if brick is rigidly snapped, we attempt to add this brick to the main assembly.
		for (int tubeind = 0; tubeind < tubes.size(); ++tubeind)
			if ( tubes[tubeind]->IsSnapped() )
				++snapCnt;
		for (int studind = 0; studind < studs.size(); ++studind)
			if (studs[studind]->IsSnapped())
				++snapCnt;

		// Bricks that are released but not snapped will not be tested against the assembly.
		if (snapCnt > 0) {
			// When target brick only had one snap, the full pose cannot be tested, only the pivot.
			// Best to avoid non-rigidly snapped bricks when authoring the target assembly.
			if (!UAssembly::_Instance->TryAddBrick(this)) 
			{
				// This brick will be destroyed in a flash of glory when it doesn't match the existing assembly.
			}
			else {
				// This brick will be silently destroyed and the brick in the existing assembly made visible.
			}
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
			overlappedBricks.push_back(enteringBrick);
			UE_LOG(LogTemp, Warning, TEXT("OnOverlapBegin enteringBrick:%s"), *FString(enteringBrick->ClientName()));
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
			overlappedBricks.remove(exitingBrick);
			UE_LOG(LogTemp, Warning, TEXT("OnOverlapEnd exitingBrick:%s"), *FString(exitingBrick->ClientName()));
			break;
		}
	}
}

void UBrick::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	// States: 
	// Seeking: No overlap with other bricks, normal grabbing mode only.
	// Snapping: Normal grabbing mode followed by tube/stud proximity matches.
	// PinSnapped: Unsnap stress test. If fail: Enter Snapping/Seeking mode. Otherwise: Pin rotate followed by tube/stud proximity matches.
	// RigidSnapped: Unsnap stress test only.

	// Move brick freely, if already snapped the brick will temporarilly be returned to unsnapped movement to allow unsnap testing.
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Try breaking any existing snaps.
	{
		for (int tubeind = 0; tubeind < tubes.size(); ++tubeind)
			tubes[tubeind]->TryBreakSnap();
		for (int studind = 0; studind < studs.size(); ++studind)
			studs[studind]->TryBreakSnap();
	}

	// If no bricks are overlapped then there can be no additional snaps.
	if (overlappedBricks.size() == 0)
		return;

	{
		// Try making new snaps.
		for (UBrick* brick : overlappedBricks) {
			// Test all local tubes with overlapped bricks studs.
			for (int tubeind = 0; tubeind < tubes.size(); ++tubeind) {
				for (int studind = 0; studind < brick->studs.size(); ++studind)
					tubes[tubeind]->TrySnap(brick->studs[studind]);
			}
			// Test all local studs with overlapped bricks tubes.
			for (int studind = 0; studind < studs.size(); ++studind) {
				for (int tubeind = 0; tubeind < brick->tubes.size(); ++tubeind)
					studs[studind]->TrySnap(brick->tubes[tubeind]);
			}
		}
	}

	{
		int snapcnt = 0;
		FTransform pivot;
		for (int tubeind = 0; tubeind < tubes.size(); ++tubeind) {
			if (tubes[tubeind]->ApplySnap(clientComponent, pivot, snapcnt))
				++snapcnt;
		}
		for (int studind = 0; studind < studs.size(); ++studind) {
			if (studs[studind]->ApplySnap(clientComponent, pivot, snapcnt))
				++snapcnt;
		}
	}
}

// Recursive method initiated from Assembly groundPlateBricks.
// We use Vodget::selectionFilter UInt16 high order bit, 0x1000
void UBrick::FindLayerBricks(int layerind)
{
}
