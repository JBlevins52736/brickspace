// Fill out your copyright notice in the Description page of Project Settings.


#include "RevealWand.h"
#include "EngineUtils.h"


URevealWand::URevealWand()
{
    SetIsReplicatedByDefault(true);
}

void URevealWand::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    // (Optional) Replicated vars would go here
}


void URevealWand::BeginPlay()
{
	Super::BeginPlay();

	selectionFilter = 0x04; // Only Architect can grab

    
}

//void URevealWand::ChangeBrickTouched()
//{
//	// Reveal brickTouched.
//	UStaticMeshComponent* mesh = Cast<UStaticMeshComponent>(brickTouched->GetAttachParent());
//	if (mesh != nullptr && mesh->Mobility == EComponentMobility::Movable)
//	{
//		// Add overlap with bricks.
//		mesh->SetMaterial(0, revealMaterial);
//	}
//}

void URevealWand::ChangeBrickTouched()
{
    AActor* brickOwner = brickTouched->GetOwner();
    if (!brickOwner) {
        UE_LOG(LogTemp, Warning, TEXT("No brick owner. function returned"));
        return;
    }

    if (!GetOwner() || !GetOwner()->HasAuthority())
    {
        Server_RevealBrick(brickOwner);
    }
    else
    {
        Multicast_SetBrickMaterial(brickOwner);
    }

}


void URevealWand::Server_RevealBrick_Implementation(AActor* Target)
{
    if (Target)
    {
        UE_LOG(LogTemp, Warning, TEXT("Server_RevealBrick called on server."));
        Multicast_SetBrickMaterial(Target);
    }
}

void URevealWand::Multicast_SetBrickMaterial_Implementation(AActor* TargetActor)
{
    if (!TargetActor) return;

    UE_LOG(LogTemp, Warning, TEXT("Multicast_SetBrickMaterial called on %s"), *GetNameSafe(this));

    UStaticMeshComponent* mesh = TargetActor->FindComponentByClass<UStaticMeshComponent>();
    if (mesh && mesh->Mobility == EComponentMobility::Movable)
    {
        mesh->SetMaterial(0, revealMaterial);
    }
}

