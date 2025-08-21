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
			// This attempts to resolve error noticed on release that could be due to an
			// additional grabber base class application that has not been followed by SolveSnaps().
			// While this, in theory, should never happen... treat this as a test.
			// If error still is observed it is in the actual snap solution
			// and the following line can be removed.
			SolveSnaps();

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
			//UE_LOG(LogTemp, Warning, TEXT("OnOverlapBegin enteringBrick:%s"), *FString(enteringBrick->ClientName()));
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
			//UE_LOG(LogTemp, Warning, TEXT("OnOverlapEnd exitingBrick:%s"), *FString(exitingBrick->ClientName()));
			break;
		}
	}
}

void UBrick::TryBreakSnaps()
{
	for (int tubeind = 0; tubeind < tubes.size(); ++tubeind)
		tubes[tubeind]->TryBreakSnap();
	for (int studind = 0; studind < studs.size(); ++studind)
		studs[studind]->TryBreakSnap();
}

void UBrick::FindSnaps()
{
	// If no bricks are overlapped then there can be no additional snaps.
	if (overlappedBricks.size() == 0)
		return;

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

void UBrick::SolveSnaps()
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
	TryBreakSnaps();

	FindSnaps();

	SolveSnaps();
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
void UBrick::Reveal(UMaterialInterface* revealMaterial, UMaterialInterface* brickMaterial)
{
	// studs and tubes will not be checked when inactive (revealed)
	isActive = false;

	// Only stationary bricks can be revealed
	UStaticMeshComponent* mesh = Cast<UStaticMeshComponent>(clientComponent);
	if (mesh != nullptr)
	{
		// Add overlap with bricks.
		solidMatchMaterial = brickMaterial;
		mesh->SetMaterial(0, revealMaterial);
		mesh->Mobility = EComponentMobility::Stationary;	// Grabber cannot grab.
	}
}

// When the assembler snaps a matching brick over a revealed brick it is made real and the snapped brick is destroyed.
// If the snapped brick doesn't match it is destroyed in an explosion and the revealed brick remains unchanged.
bool UBrick::TryMatch(UBrick* assemblerBrick)
{
	// First test: The bricks should be the same type.
	if (shortName != assemblerBrick->shortName)
		return false;

	// Second test: The bricks client meshes should have the same color.
	UStaticMeshComponent* othMesh = Cast<UStaticMeshComponent>(assemblerBrick->clientComponent);
	if (othMesh == nullptr)
		return false;


	UStaticMeshComponent* mesh = Cast<UStaticMeshComponent>(clientComponent);
	if (mesh == nullptr)
		return false;

	if (mesh->GetMaterial(0)->GetMaterial() != assemblerBrick->solidMatchMaterial) {

		UE_LOG(LogTemp, Warning, TEXT("Failed Materials equal: %s %s"),
			*(mesh->GetMaterial(0)->GetMaterial()->GetPathName()), *(assemblerBrick->solidMatchMaterial->GetPathName()));
		return false;
	}

	// Final test: The bricks should have the same position and orientation

	if (poseIsUnique)
	{
		// For bricks where only one valid orientation is possible, matching tubes must also have the same index. 
		for (int tubeind = 0; tubeind < tubes.size(); ++tubeind) {
			FVector pos = tubes[tubeind]->GetComponentLocation();
			FVector othpos = assemblerBrick->tubes[tubeind]->GetComponentLocation();
			if (!pos.Equals(othpos, 2.1)) // WHY ARE THEY OFF 2.1cm in world space when snapped in the opposite direction?!
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed tubeIndexMustMatch:"));
				return false;
			}
		}
	}
	else {
		// For simple bricks, there might be multiple ways a brick is placed that looks identical.
		// All tubes should find matching tubes just not with the same tubes vector index.

		int matched = 0;
		int tubesSnapped = 0;
		for (int tubeind = 0; tubeind < tubes.size(); ++tubeind) {
			if (assemblerBrick->tubes[tubeind]->IsSnapped())
				++tubesSnapped;
			FVector pos = tubes[tubeind]->GetComponentLocation();
			for (int othtubeind = 0; othtubeind < assemblerBrick->tubes.size(); ++othtubeind) {
				FVector othpos = assemblerBrick->tubes[othtubeind]->GetComponentLocation();
				if (pos.Equals(othpos, 2.1)) // WHY ARE THEY OFF 2.1cm in world space when snapped in the opposite direction?!
					++matched;
			}
		}

		// If only one tube is snapped then possible orientations are infinite.
		if (tubesSnapped > 1 && matched != tubes.size())
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed pose: Not all tubes matched"));
			return false;
		}

	}

	UE_LOG(LogTemp, Warning, TEXT("Bricks match:"));

	// Make assemblerBrick active and solid.
	assemblerBrick->isActive = true;
	othMesh->SetMaterial(0, mesh->GetMaterial(0));

	UAssembly* assembly = Cast<UAssembly>(assemblerBrick->clientComponent->GetAttachParent());
	if (assembly != nullptr) {
		assembly->TryAdvanceLayer();
	}
	return true;
}