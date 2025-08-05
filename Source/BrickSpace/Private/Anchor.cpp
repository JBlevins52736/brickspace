// Fill out your copyright notice in the Description page of Project Settings.


#include "Anchor.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"



void UAnchor::BeginPlay()
{
    Super::BeginPlay();
}




void UAnchor::OrientToAnchor(AActor* AnchorActor)
{
    if (!AnchorActor) return;

    FTransform AnchorTransform = AnchorActor->GetActorTransform();
    FTransform ActorTransform = GetOwner()->GetActorTransform();

  
    FTransform RelativeTransform = AnchorTransform.Inverse() * ActorTransform;

    FVector RelativeLocation = RelativeTransform.GetLocation();
    FRotator RelativeRotation = RelativeTransform.Rotator();

    
    Server_OrientToAnchor(RelativeLocation, RelativeRotation);
}

void UAnchor::Server_OrientToAnchor_Implementation(const FVector& NewLocation, const FRotator& NewRotation)
{
    if (AActor* Owner = GetOwner())
    {
        Owner->SetActorLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);
        Owner->SetActorRotation(NewRotation, ETeleportType::TeleportPhysics);

        Multicast_ApplyAnchorTransform(NewLocation, NewRotation);
    }

}

void UAnchor::Multicast_ApplyAnchorTransform_Implementation(const FVector& NewLocation, const FRotator& NewRotation)
{
    if (AActor* Owner = GetOwner())
    {
        Owner->SetActorLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);
        Owner->SetActorRotation(NewRotation, ETeleportType::TeleportPhysics);
    }
}