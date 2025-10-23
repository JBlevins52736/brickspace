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
}

void UWallBrick::ForePinch(USelector* selector, bool state)
{
	Super::ForePinch(selector, state);

	if (!selector || !clientComponent)
		return;

	clientComponentCloned = nullptr;

	if (state)
	{
		InitialRelativeTransform = clientComponent->GetRelativeTransform();
		InitialWorldTransform = GetOwner()->GetActorTransform();
	}
	else if (!bThresholdReached)
	{
		clientComponent->SetRelativeTransform(InitialRelativeTransform);

		ABrickSpacePawn* pawn = Cast<ABrickSpacePawn>(selector->GetOwner());
		if (!pawn->HasAuthority())
			pawn->Server_MoveRelative(clientComponent, InitialRelativeTransform);
	}
}

void UWallBrick::CloneWallBrick(const FTransform &worldTransform)
{
	AActor* TargetActor = GetOwner();
	UWorld* World = TargetActor->GetWorld();
	if (!World) return;

	// Get the wall parent component
	if (clientComponent == nullptr) {
		// WTF: Why wasn't this initialized in Vodget base class? 
		// This only occurs when spawned on a remote client from the listen server client.

		UE_LOG(LogTemp, Warning, TEXT("OnRep_Parent clientComponent null"));
		clientComponent = GetAttachParent();
	}
	USceneComponent *WallComponent = clientComponent->GetAttachParent();

	//Spawn the new brick at a neutral, irrelevant location.
	FActorSpawnParameters params;
	params.Owner = WallComponent->GetOwner();
	AActor* ClonedBrick = World->SpawnActor<AActor>(TargetActor->GetClass(), worldTransform, params);
	if (!ClonedBrick) return;

	ClonedBrick->AttachToActor(WallComponent->GetOwner(), FAttachmentTransformRules::KeepWorldTransform);

	// Mark the original brick as removed/cloned
	bThresholdReached = true;
	clientComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
}

void UWallBrick::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//MoveCloned();

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
		CloneWallBrick(InitialWorldTransform);
	else
		pawn->Server_CloneWallBrick(this, InitialWorldTransform);

}

void UWallBrick::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//DOREPLIFETIME(UWallBrick, InitialRelativeTransform);
	DOREPLIFETIME(UWallBrick, bThresholdReached);
}
