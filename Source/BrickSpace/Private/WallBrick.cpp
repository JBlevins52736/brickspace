#include "WallBrick.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Actor.h"
#include "BrickActor.h"
#include "Selector.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h" // Required for DOREPLIFETIME

#include "BrickSpacePawn.h"
#include <Kismet/GameplayStatics.h>

static const FName TAG_SpawnWall(TEXT("spawn wall"));

void UWallBrick::BeginPlay()
{
	Super::BeginPlay();

	//if (GetOwner()->HasAuthority())

	if (clientComponent == nullptr) {

		UE_LOG(LogTemp, Warning, TEXT("OnRep_Parent clientComponent null"));
		clientComponent = GetAttachParent();
	}

	//if (clientComponent) {

	//	InitialTransform = clientComponent->GetComponentTransform();

	//	if (clientComponent->GetAttachParent())
	//	{

	//		InitialRelativeTransform = clientComponent->GetRelativeTransform();

	//	}
	//}




}

void UWallBrick::ForePinch(USelector* selector, bool state)
{
	Super::ForePinch(selector, state);


	if (!selector || !grabbingSelector || !clientComponent)
		return;

	if (state) {
		InitialRelativeTransform = clientComponent->GetRelativeTransform();
	}
	else if (!bThresholdReached )
	{
		ABrickSpacePawn* pawn = Cast<ABrickSpacePawn>(grabbingSelector->GetOwner());
		if (pawn->HasAuthority())
			clientComponent->SetRelativeTransform(InitialRelativeTransform);9
		else
			pawn->Server_MoveRelative(clientComponent, InitialRelativeTransform); 
	}
}

void UWallBrick::Server_CloneWallBrick_Implementation( FTransform initialTransform )
{
	AActor* TargetActor = GetOwner();
	UWorld* World = TargetActor->GetWorld();
	if (!World) return;

	InitialRelativeTransform = initialTransform;

	// Get the wall parent component
	if (clientComponent == nullptr) {
		// WTF: Why wasn't this initialized in Vodget base class? 
		// This only occurs when spawned on a remote client from the listen server client.

		UE_LOG(LogTemp, Warning, TEXT("OnRep_Parent clientComponent null"));
		clientComponent = GetAttachParent();
	}
	USceneComponent* WallComponent = clientComponent ? clientComponent->GetAttachParent() : nullptr;
	if (!WallComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("UWallBrick failed to find parent component for attachment!"));
		return;
	}

	//Spawn the new brick at a neutral, irrelevant location.
	AActor* clonedBrick = World->SpawnActor<AActor>(TargetActor->GetClass(), FTransform::Identity);
	if (!clonedBrick) return;

	//// Ensure the new brick has movement replication enabled.
	//clonedBrick->SetReplicates(true);
	//clonedBrick->SetReplicateMovement(true);

	//Attach the cloned brick to the WallComponent.
	clonedBrick->GetRootComponent()->AttachToComponent(
		WallComponent,
		FAttachmentTransformRules::KeepRelativeTransform
	);

	clonedBrick->GetRootComponent()->SetRelativeTransform(InitialRelativeTransform);

	//UWallBrick* clonedWallBrick = clonedBrick->FindComponentByClass<UWallBrick>();
	//if (clonedWallBrick)
	//{
	//	clonedWallBrick->InitialRelativeTransform = InitialRelativeTransform;

	//	UE_LOG(LogTemp, Log, TEXT("Set cloned brick's InitialRelativeTransform for replication: %s"),
	//		*InitialRelativeTransform.GetLocation().ToString());
	//}

	UE_LOG(LogTemp, Log, TEXT("Cloned brick attached to parent: %s, Final Relative Pos: %s"),
		*WallComponent->GetName(), *InitialRelativeTransform.GetLocation().ToString());

	// Mark the original brick as removed/cloned
	bThresholdReached = true;
	if (bThresholdReached)
	{
		clientComponent->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	}
}
void UWallBrick::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bThresholdReached || !grabbingSelector) return;

	// We must be grabbing and pulling away from the wall.
	const float Dist = FVector::Dist(
		clientComponent->GetRelativeLocation(),
		InitialRelativeTransform.GetLocation()
	);

	// If within threshold, do nothing.
	if (Dist <= DistanceThreshold)
		return;

	// Threshold reached, clone the brick.
	bThresholdReached = true;

	ABrickSpacePawn* pawn = Cast<ABrickSpacePawn>(grabbingSelector->GetOwner());
	if (pawn->HasAuthority())
		Server_CloneWallBrick_Implementation(InitialRelativeTransform);                // If already on server, just spawn.
	else
		pawn->Server_CloneWallBrick(this, InitialRelativeTransform);    // Ask server to spawn.

}

//void UWallBrick::OnRep_InitialRelativeTransform()
//{
//	// This runs on clients when InitialRelativeTransform is replicated
//	if (clientComponent && !GetOwner()->HasAuthority())
//	{
//		// Apply the replicated relative transform
//		clientComponent->SetRelativeTransform(InitialRelativeTransform);
//
//		UE_LOG(LogTemp, Log, TEXT("Client applied replicated relative transform: %s"),
//			*InitialRelativeTransform.GetLocation().ToString());
//	}
//}

void UWallBrick::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//DOREPLIFETIME(UWallBrick, InitialRelativeTransform);
	DOREPLIFETIME(UWallBrick, bThresholdReached);
}
