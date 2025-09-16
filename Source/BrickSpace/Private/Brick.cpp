// Fill out your copyright notice in the Description page of Project Settings.

#include "Brick.h"
#include "BrickActor.h"
#include "AssemblyActor.h"
#include "Assembly.h"
#include "Net/UnrealNetwork.h" // Required for DOREPLIFETIME
#include "BrickSpacePlayerState.h"
#include <Kismet/GameplayStatics.h>
#include "Selector.h"

void UBrick::BeginPlay()
{
	Super::BeginPlay();

	UStaticMeshComponent* mesh = Cast<UStaticMeshComponent>(clientComponent);
	if (mesh != nullptr)
		brickMaterial = mesh->GetMaterial(0);

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
			bool attemptOverlapWithTranslucent = false;
			bool overlapMatchFound = false;
			for (UBrick* brick : overlappedBricks) {
				if (!brick->isSolid) {
					attemptOverlapWithTranslucent = true;
					if (TryMatch(brick))
						overlapMatchFound = true;
				}
			}

			if (attemptOverlapWithTranslucent) {
				if (!overlapMatchFound)
					DoExplodeMismatchedEffect();

				// Bricks off the wall are always deleted.
				// When matched the translucent brick is made solid and one brick in the layer has been solved.
				ABrickActor* brickActor = Cast<ABrickActor>(GetOwner());
				brickActor->Server_Delete();
				//if (playerState && GetOwner()) {
				//	playerState->Server_DeleteActor(GetOwner());
				//}
				//else {
				//	UE_LOG(LogTemp, Error, TEXT("playerState or owner null when deleting."));
				//}
				//GetOwner()->Destroy(true, true); //  Destroy();	
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

		if (brick->isSolid) {
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

//static float elapsedTick = 0.0;

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

	//elapsedTick += DeltaTime;

	//if (elapsedTick > 100.0) {

	// Apply final brick location to server. 
	ABrickActor* brickActor = Cast<ABrickActor>(GetOwner());
	brickActor->Server_Move(GetOwner(), clientComponent->GetComponentTransform());

	//	elapsedTick -= 100.0;
	//}
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
	return brickMaterial;
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

void UBrick::OnRep_Parent()
{
	if (clientComponent == nullptr) {
		// WTF: Why wasn't this initialized in Vodget base class? 
		// This only occurs when spawned on a remote client from the listen server client.

		UE_LOG(LogTemp, Warning, TEXT("OnRep_Parent clientComponent null"));
		clientComponent = GetAttachParent();
	}

	if (assemblyActor == nullptr)
		UE_LOG(LogTemp, Warning, TEXT("OnRep_Parent: is null"));

	if (clientComponent != nullptr && assemblyActor != nullptr) {
		UAssembly* assembly = assemblyActor->FindComponentByClass<UAssembly>();
		//UAssembly* assembly = assemblyActor->assembly;
		if (assembly != nullptr) {
			UE_LOG(LogTemp, Warning, TEXT("OnRep_Parent: changing parent"));
			clientComponent->AttachToComponent(assembly, FAttachmentTransformRules::KeepRelativeTransform);
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("OnRep_Parent: attach failed"));

	}
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

//// The architect touches bricks to reveal them.
//void UBrick::Reveal(UMaterialInterface* revealMaterial, UMaterialInterface* brickMaterial)
//{
//	// studs and tubes will not be checked when inactive (revealed)
//	isSolid = false;
//
//	// Only stationary bricks can be revealed
//	UStaticMeshComponent* mesh = Cast<UStaticMeshComponent>(clientComponent);
//	if (mesh != nullptr)
//	{
//		// Add overlap with bricks.
//		solidMatchMaterial = brickMaterial;
//		mesh->SetMaterial(0, revealMaterial);
//		mesh->Mobility = EComponentMobility::Stationary;	// Grabber cannot grab.
//	}
//}

// When the assembler snaps a matching brick over a revealed brick it is made real and the snapped brick is destroyed.
// If the snapped brick doesn't match it is destroyed in an explosion and the revealed brick remains unchanged.
bool UBrick::TryMatch(UBrick* assemblerBrick)
{
	// First test: The bricks should be the same type.
	if (shortName != assemblerBrick->shortName)
		return false;

	// Second test: The bricks client meshes should have the same color.
	UStaticMeshComponent* assemblerBrickMesh = Cast<UStaticMeshComponent>(assemblerBrick->clientComponent);
	if (assemblerBrickMesh == nullptr)
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

				if (tubes.size() == 1) {
					float dist = FVector::Distance(pos, othpos);
					UE_LOG(LogTemp, Warning, TEXT("SingleTubeDist: %f"), dist);
				}

				if (pos.Equals(othpos, 2.1)) // WHY ARE THEY OFF 2.1cm in world space when snapped in the opposite direction?!
					++matched;
			}
		}

		// If only one tube is snapped then possible orientations are infinite.
		if (/*tubesSnapped > 1 && */matched != tubes.size())
		{
			//UE_LOG(LogTemp, Warning, TEXT("Failed pose: Not all tubes matched"));
			return false;
		}

	}

	UE_LOG(LogTemp, Warning, TEXT("Bricks match:"));

	// Make assemblerBrick active and solid.
	//if (playerState != nullptr && assemblerBrickMesh != nullptr && assemblerBrickMesh->GetOwner() != nullptr) {
	//	playerState->Server_ChangeMaterial(assemblerBrickMesh->GetOwner(), mesh->GetMaterial(0), true);
	//}
	ABrickActor* brickActor = Cast<ABrickActor>(assemblerBrickMesh->GetOwner());
	if (!brickActor) {
		UE_LOG(LogTemp, Warning, TEXT("Bricks match: But assemblerBrick not a brickActor?"));
		return false;
	}

	//// We probably don't have authority over the translucent brick that the grabbed brick just collided with.
	//if (!brickActor->HasAuthority()) {
	//	if (playerState == nullptr) {
	//		APlayerState* PlayerStateAtIndex0 = UGameplayStatics::GetPlayerState(GetWorld(), 0);
	//		playerState = Cast<ABrickSpacePlayerState>(PlayerStateAtIndex0);
	//	}
	//	
	//	playerState->Server_Own(brickActor, playerState);
	//}
	if (playerState == nullptr) {
		APlayerState* PlayerStateAtIndex0 = UGameplayStatics::GetPlayerState(GetWorld(), 0);
		playerState = Cast<ABrickSpacePlayerState>(PlayerStateAtIndex0);
	}
	playerState->Server_ChangeMaterial(brickActor, mesh->GetMaterial(0), true);

	if (!brickActor->brick) {
		UE_LOG(LogTemp, Warning, TEXT("Bricks match: But brickActor->brick is null?"));
		return false;
	}

	// Notify server to check if layer is solved and either add the next layer or launch the rocket.
	//AActor* groundplate = assemblerBrick->clientComponent->GetAttachParent()->GetOwner();

	if (playerState == nullptr) {
		APlayerState* PlayerStateAtIndex0 = UGameplayStatics::GetPlayerState(GetWorld(), 0);
		playerState = Cast<ABrickSpacePlayerState>(PlayerStateAtIndex0);
	}

	//if (!assemblyActor)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("assemblyActor null before Brick TryAdvanceLayer call"));
	//}
	if (playerState != nullptr && assemblerBrick->assemblyActor ) {
		playerState->Server_TryAdvanceLayer(assemblerBrick->assemblyActor);
	}

	//if (!brickActor->brick->assemblyActor)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Bricks match: But brickActor->brick has no assemblyActor?"));
	//	return false;
	//}

	//brickActor->brick->assemblyActor->Server_TryAdvanceLayer();
	return true;
}

void UBrick::OnRep_Material()
{
	if (clientComponent == nullptr) {
		// WTF: Why wasn't this initialized in Vodget base class? 
		// This only occurs when spawned on a remote client from the listen server client.

		UE_LOG(LogTemp, Warning, TEXT("OnRep_Material clientComponent null"));
		clientComponent = GetAttachParent();
	}
	UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(clientComponent);
	if (MeshComp != nullptr) {
		MeshComp->SetMaterial(0, brickMaterial);
		//UE_LOG(LogTemp, Warning, TEXT("OnRep_Material: setting material %s"), *(brickMaterial->GetFName()).ToString());
	}
}

void UBrick::OnRep_Grabbable()
{
	if (clientComponent == nullptr)
	{
		// See OnRep_Material comments above
		UE_LOG(LogTemp, Warning, TEXT("OnRep_Grabbable clientComponent null"));
		clientComponent = GetAttachParent();
	}

	UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(clientComponent);
	if (MeshComp != nullptr) {
		MeshComp->Mobility = (isGrabbable) ? EComponentMobility::Movable : EComponentMobility::Stationary;
		//UE_LOG(LogTemp, Warning, TEXT("OnRep_Grabbable"));
	}
}

void UBrick::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UBrick, brickMaterial);
	DOREPLIFETIME(UBrick, solidMatchMaterial);
	DOREPLIFETIME(UBrick, isSolid);
	DOREPLIFETIME(UBrick, isGrabbable);
	DOREPLIFETIME(UBrick, assemblyActor);

}