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
        // Store the authoritative world transform (no longer used for spawning, but useful for reset)
        InitialTransform = clientComponent->GetComponentTransform();

        if (clientComponent && clientComponent->GetAttachParent())
        {
            // Store and replicate the authoritative relative transform
            InitialRelativeTransform = clientComponent->GetRelativeTransform();
            // This replicated value is sent to all clients via DOREPLIFETIME.
        }
    }
    // If the client is not the authority, they will receive the correct FTransform via replication.

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

//void UWallBrick::Server_CloneWallBrick_Implementation(const FTransform& onWallTransform)
//{
//    /*  AActor* TargetActor = GetOwner();
//      AActor* clonedBrick = TargetActor->GetWorld()->SpawnActor<AActor>(TargetActor->GetClass(), onWallTransform);
//      bThresholdReached = true;*/
//    AActor* TargetActor = GetOwner();
//    UWorld* World = TargetActor->GetWorld();
//    if (!World) return;
//
//    // We assume the desired spawn location is the current world location of the brick 
//    // before it gets pulled, which is stored in InitialTransform.
//    FTransform SpawnWorldTransform = InitialTransform;
//
//    // 1. Spawn the new brick at the saved WORLD location
//    AActor* clonedBrick = World->SpawnActor<AActor>(TargetActor->GetClass(), SpawnWorldTransform);
//    if (!clonedBrick) return;
//
//    // 2. FIND THE MOVING PARENT (The Wall Scene Component)
//    USceneComponent* WallComponent = clientComponent ? clientComponent->GetAttachParent() : nullptr;
//
//    if (WallComponent)
//    {
//        //// 3. ATTACH the cloned brick's root component to the WallComponent
//        //clonedBrick->GetRootComponent()->AttachToComponent(
//        //    WallComponent,
//        //    FAttachmentTransformRules::KeepWorldTransform // Keep its current world position initially
//        //);
//
//        //// 4. SET THE RELATIVE TRANSFORM
//        //// Now that it's attached, set its relative transform to the saved value.
//        //clonedBrick->GetRootComponent()->SetRelativeTransform(InitialRelativeTransform);
//
//        //UE_LOG(LogTemp, Log, TEXT("Cloned brick attached to parent: %s, Relative Pos: %s"),
//        //    *WallComponent->GetName(), *InitialRelativeTransform.GetLocation().ToString());
//        if (WallComponent)
//        {
//            // *** FIX STARTS HERE ***
//
//            // 3. ATTACH the cloned brick's root component to the WallComponent
//            // Use the SnapToTarget rule and set the transform to the desired RELATIVE transform.
//            // This is the cleanest way to set a replicated relative position.
//            clonedBrick->GetRootComponent()->AttachToComponent(
//                WallComponent,
//                FAttachmentTransformRules::SnapToTargetNotIncludingScale // Snap location/rotation
//            );
//
//            // 4. SET THE RELATIVE TRANSFORM
//            // Now that it's attached and snapped, explicitly set the final relative transform.
//            // This ensures the replicated InitialRelativeTransform is the absolute truth for the brick's position.
//            clonedBrick->GetRootComponent()->SetRelativeTransform(InitialRelativeTransform);
//
//            // *** FIX ENDS HERE ***
//
//            UE_LOG(LogTemp, Log, TEXT("Cloned brick attached to parent: %s, Relative Pos: %s"),
//                *WallComponent->GetName(), *InitialRelativeTransform.GetLocation().ToString());
//        }
//    }
//    else
//    {
//        UE_LOG(LogTemp, Error, TEXT("UWallBrick failed to find parent component for attachment!"));
//    }
//
//    // Mark the original brick as removed/cloned
//    bThresholdReached = true;
//    if (bThresholdReached) 
//    {
//        clientComponent->DetachFromParent();
//    }
//}
//void UWallBrick::Server_CloneWallBrick_Implementation(const FTransform& onWallTransform)
//{
//    AActor* TargetActor = GetOwner();
//    UWorld* World = TargetActor->GetWorld();
//    if (!World) return;
//
//    // 1. Get the authoritative wall parent component
//    USceneComponent* WallComponent = clientComponent ? clientComponent->GetAttachParent() : nullptr;
//    if (!WallComponent)
//    {
//        UE_LOG(LogTemp, Error, TEXT("UWallBrick failed to find parent component for attachment!"));
//        return;
//    }
//
//    // 2. Spawn the new brick at a neutral, irrelevant location.
//    // The location/rotation is irrelevant because it will be immediately attached and snapped.
//    AActor* clonedBrick = World->SpawnActor<AActor>(TargetActor->GetClass(), FTransform::Identity);
//
//    if (!clonedBrick) return;
//
//    // --- CRITICAL REPLICATION STEPS ---
//
//    // Ensure the new brick has movement replication enabled.
//    clonedBrick->SetReplicates(true);
//    clonedBrick->SetReplicateMovement(true);
//
//    // 3. ATTACH and SNAP: Attach the cloned brick to the WallComponent.
//    // Use the KEEP_RELATIVE_TRANSFORM rule because the component is spawned at Identity 
//    // and we are about to set its correct relative transform.
//    clonedBrick->GetRootComponent()->AttachToComponent(
//        WallComponent,
//        FAttachmentTransformRules::KeepRelativeTransform // Keep Relative (Identity) before we set it below
//    );
//
//    // 4. SET THE FINAL RELATIVE TRANSFORM: This is the authoritative, replicated position.
//    clonedBrick->GetRootComponent()->SetRelativeTransform(InitialRelativeTransform);
//
//    // --- END CRITICAL REPLICATION STEPS ---
//
//    UE_LOG(LogTemp, Log, TEXT("Cloned brick attached to parent: %s, Final Relative Pos: %s"),
//        *WallComponent->GetName(), *InitialRelativeTransform.GetLocation().ToString());
//
//    // Mark the original brick as removed/cloned
//    bThresholdReached = true;
//    if (bThresholdReached)
//    {
//        clientComponent->DetachFromParent();
//    }
//}

void UWallBrick::Server_CloneWallBrick_Implementation(const FTransform& onWallTransform)
{
    AActor* TargetActor = GetOwner();
    UWorld* World = TargetActor->GetWorld();
    if (!World) return;

    // 1. Get the authoritative wall parent component
    USceneComponent* WallComponent = clientComponent ? clientComponent->GetAttachParent() : nullptr;
    if (!WallComponent)
    {
        UE_LOG(LogTemp, Error, TEXT("UWallBrick failed to find parent component for attachment!"));
        return;
    }

    // 2. Spawn the new brick at a neutral, irrelevant location.
    AActor* clonedBrick = World->SpawnActor<AActor>(TargetActor->GetClass(), FTransform::Identity);
    if (!clonedBrick) return;

    // Ensure the new brick has movement replication enabled.
    clonedBrick->SetReplicates(true);
    clonedBrick->SetReplicateMovement(true);

    // 3. ATTACH: Attach the cloned brick to the WallComponent.
    clonedBrick->GetRootComponent()->AttachToComponent(
        WallComponent,
        FAttachmentTransformRules::KeepRelativeTransform
    );

    // 4. SET THE FINAL RELATIVE TRANSFORM on the component
    clonedBrick->GetRootComponent()->SetRelativeTransform(InitialRelativeTransform);

    // *** CRITICAL FIX: Set the replicated property on the cloned brick's UWallBrick component ***
    UWallBrick* clonedWallBrick = clonedBrick->FindComponentByClass<UWallBrick>();
    if (clonedWallBrick)
    {
        // This is what gets replicated to clients!
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
        clientComponent->DetachFromParent();
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
