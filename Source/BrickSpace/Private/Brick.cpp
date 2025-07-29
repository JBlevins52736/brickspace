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

			// Check to see if we match any non active (revealed) bricks we overlapped
			for (UBrick* brick : overlappedBricks) {
				if (!brick->isActive) {
					if (TryMatch(brick)) {

					}
					else {
						DoExplodeMismatchedEffect();
					}
					GetOwner()->Destroy(true, true); //  Destroy();
				}
			}
		}
	}

}

void UBrick::DoExplodeMismatchedEffect()
{
}

void UBrick::DoAcceptMatchWithRevealedBrick()
{
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

			if (brick->isActive) {
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

FVector UBrick::GetLocation()
{
	return clientComponent->GetRelativeLocation();
}

FQuat UBrick::GetQuat()
{
	return clientComponent->GetRelativeRotation().Quaternion();
}

UMaterialInterface* UBrick::GetMaterial()
{
	UStaticMeshComponent* mesh = Cast<UStaticMeshComponent>(clientComponent);
	if (mesh != nullptr)
		return mesh->GetMaterial(0);
	return nullptr;
}

bool UBrick::TryReparent(USceneComponent* pnt, std::vector<UBrick*>& layerBricks)
{
	// Note: After the initial clientComponent (brick) is reparented to pnt(Assembly),
	// clientComponent is set to pnt(Assembly) so that Grabber now grabs entire group instead of a single brick.
	if (clientComponent->Mobility != EComponentMobility::Movable || clientComponent->GetAttachParent() == pnt)
		return false;

	clientComponent->AttachToComponent(pnt, FAttachmentTransformRules::KeepWorldTransform);
	//clientComponent = pnt;
	layerBricks.push_back(this);
	return true;
}

// Recursive method initiated from Assembly groundPlateBricks.
// We use Vodget::selectionFilter UInt16 high order bit, 0x1000
void UBrick::ReparentConnectedBricks(USceneComponent* pnt, std::vector<UBrick*>& layerBricks)
{
	// Note: This brick is either already in the assembly or it is an unmovable groundplane brick.
	// Unmovable bricks are immediatly rejected in the TryReparent(...) method.
	int snapcnt = 0;
	for (int tubeind = 0; tubeind < tubes.size(); ++tubeind)
		if (tubes[tubeind]->snappedTo != nullptr)
			tubes[tubeind]->snappedTo->brick->TryReparent(pnt, layerBricks);
	for (int studind = 0; studind < studs.size(); ++studind)
		if (studs[studind]->snappedTo != nullptr)
			studs[studind]->snappedTo->brick->TryReparent(pnt, layerBricks);
}

// The architect touches bricks to reveal them.
void UBrick::Reveal(UMaterialInterface* revealMaterial)
{
	// studs and tubes will not be checked when inactive (revealed)
	isActive = false;

	// Only stationary bricks can be revealed
	UStaticMeshComponent* mesh = Cast<UStaticMeshComponent>(clientComponent);
	if (mesh != nullptr)
	{
		// Add overlap with bricks.
		mesh->SetMaterial(0, revealMaterial);
		mesh->Mobility = EComponentMobility::Stationary;	// Grabber cannot grab.
	}
}

// When the assembler snaps a matching brick over a revealed brick it is made real and the snapped brick is destroyed.
// If the snapped brick doesn't match it is destroyed in an explosion and the revealed brick remains unchanged.
bool UBrick::TryMatch(UBrick* assemblerBrick)
{
	UStaticMeshComponent* othMesh = Cast<UStaticMeshComponent>(assemblerBrick->clientComponent);
	if (othMesh == nullptr)
		return false;


	UStaticMeshComponent* mesh = Cast<UStaticMeshComponent>(clientComponent);
	if (mesh == nullptr )
		return false;

	//if (mesh->GetMaterial(0)->GetMaterial() != othMesh->GetMaterial(0)->GetMaterial())
	//	return false;

	FTransform pose = clientComponent->GetComponentTransform();
	FTransform othpose = assemblerBrick->clientComponent->GetComponentTransform();
	if (!FTransform::AreRotationsEqual(pose, othpose) || !FTransform::AreTranslationsEqual(pose, othpose))
		return false;

	// We have a full match.
	
	// Make assemblerBrick active and solid.
	assemblerBrick->isActive = true;
	othMesh->SetMaterial(0, mesh->GetMaterial(0));

	UAssembly* assembly = Cast<UAssembly>(assemblerBrick->clientComponent->GetAttachParent() );
	if (assembly != nullptr) {
		assembly->TryAdvanceLayer();
	}
	return true;
}
