#include "WallBrick.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Actor.h"
#include "BrickActor.h"
#include "Selector.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h" // Required for DOREPLIFETIME

#include "BrickSpacePawn.h"
//#include "BrickSpacePlayerState.h"
#include <Kismet/GameplayStatics.h>

static const FName TAG_SpawnWall(TEXT("spawn wall"));

void UWallBrick::BeginPlay()
{
    Super::BeginPlay();
    //InitialTransform = clientComponent->GetComponentTransform();
    InitialTransform = clientComponent->GetComponentTransform();
}

void UWallBrick::ForePinch(USelector* selector, bool state)
{
    //InitialTransform = clientComponent->GetComponentTransform();
    Super::ForePinch(selector, state);

    if(!state && !bThresholdReached )
    {
        ABrickSpacePawn* pawn = Cast<ABrickSpacePawn>(grabbingSelector->GetOwner());
        if (pawn->HasAuthority())
			GetOwner()->SetActorTransform(InitialTransform);    // If already on server, just reset transform.
        else
			pawn->Server_Move(GetOwner(), InitialTransform);    // Ask server to reset transform.
    }
}

void UWallBrick::Server_CloneWallBrick(const FTransform& onWallTransform)
{
    AActor* TargetActor = GetOwner();
    AActor* clonedBrick = TargetActor->GetWorld()->SpawnActor<AActor>(TargetActor->GetClass(), onWallTransform);

    // When we set this from the server it will replicate to all clients.
    //UWallBrick* wallBrick = clonedBrick->FindComponentByClass<UWallBrick>();
    //if (wallBrick != nullptr)
    //    wallBrick->bThresholdReached = true;

    bThresholdReached = true;
}

void UWallBrick::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    if (bThresholdReached || !grabbingSelector) return;

	// We must be grabbing and pulling away from the wall.
    const float Dist = FVector::Dist(
        clientComponent->GetComponentLocation(),
        InitialTransform.GetLocation()
    );

	// If within threshold, do nothing.
    if (Dist <= DistanceThreshold)
        return;

	// Threshold reached, clone the brick.
    bThresholdReached = true;

    ABrickSpacePawn* pawn = Cast<ABrickSpacePawn>(grabbingSelector->GetOwner());
    if (pawn->HasAuthority())
        Server_CloneWallBrick(InitialTransform);    // If already on server, just spawn.
    else
        pawn->Server_CloneWallBrick(this, InitialTransform);    // Ask server to spawn.

}

void UWallBrick::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UWallBrick, bThresholdReached);
}
