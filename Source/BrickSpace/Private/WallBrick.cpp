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
    if (GetOwner()->HasAuthority())
    {
       
        InitialTransform = clientComponent->GetComponentTransform();

        if (clientComponent && clientComponent->GetAttachParent())
        {
          
            InitialRelativeTransform = clientComponent->GetRelativeTransform();
           
        }
    }


}

void UWallBrick::ForePinch(USelector* selector, bool state)
{
    Super::ForePinch(selector, state);

    if(!state && !bThresholdReached && selector && grabbingSelector)
    {
        ABrickSpacePawn* pawn = Cast<ABrickSpacePawn>(grabbingSelector->GetOwner());
        if (pawn->HasAuthority())
			GetOwner()->SetActorTransform(InitialTransform);    // If already on server, just reset transform.
        else
			pawn->Server_Move(GetOwner(), InitialTransform);    // Ask server to reset transform.
    }
}

void UWallBrick::Server_CloneWallBrick_Implementation(const FTransform& onWallTransform)
{
    AActor* TargetActor = GetOwner();
    UWorld* World = TargetActor->GetWorld();
    if (!World) return;

    // Get the wall parent component
    USceneComponent* WallComponent = clientComponent ? clientComponent->GetAttachParent() : nullptr;
    if (!WallComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("UWallBrick failed to find parent component for attachment!"));
        return;
    }

    //Spawn the new brick at a neutral, irrelevant location.
    AActor* clonedBrick = World->SpawnActor<AActor>(TargetActor->GetClass(), FTransform::Identity);
    if (!clonedBrick) return;

    // Ensure the new brick has movement replication enabled.
    clonedBrick->SetReplicates(true);
    clonedBrick->SetReplicateMovement(true);

    //Attach the cloned brick to the WallComponent.
    clonedBrick->GetRootComponent()->AttachToComponent(
        WallComponent,
        FAttachmentTransformRules::KeepRelativeTransform
    );

    clonedBrick->GetRootComponent()->SetRelativeTransform(InitialRelativeTransform);

    UWallBrick* clonedWallBrick = clonedBrick->FindComponentByClass<UWallBrick>();
    if (clonedWallBrick)
    {
        clonedWallBrick->InitialRelativeTransform = InitialRelativeTransform;

        UE_LOG(LogTemp, Log, TEXT("Set cloned brick's InitialRelativeTransform for replication: %s"),
            *InitialRelativeTransform.GetLocation().ToString());
    }

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
        Server_CloneWallBrick_Implementation(InitialTransform);                // If already on server, just spawn.
    else
        pawn->Server_CloneWallBrick(this, InitialTransform);    // Ask server to spawn.

}

void UWallBrick::OnRep_InitialRelativeTransform()
{
    // This runs on clients when InitialRelativeTransform is replicated
    if (clientComponent && !GetOwner()->HasAuthority())
    {
        // Apply the replicated relative transform
        clientComponent->SetRelativeTransform(InitialRelativeTransform);

        UE_LOG(LogTemp, Log, TEXT("Client applied replicated relative transform: %s"),
            *InitialRelativeTransform.GetLocation().ToString());
    }
}

void UWallBrick::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UWallBrick, InitialRelativeTransform);
    DOREPLIFETIME(UWallBrick, bThresholdReached);
}
