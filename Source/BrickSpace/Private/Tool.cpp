// Fill out your copyright notice in the Description page of Project Settings.


#include "Tool.h"
#include <Kismet/GameplayStatics.h>
#include "BrickSpacePlayerState.h"
#include "Selector.h"


void UTool::ForePinch(USelector* selector, bool state)
{
	if (clientComponent->Mobility != EComponentMobility::Movable &&
		grabbingSelector != nullptr && grabbingSelector != selector)
		return;

	Super::ForePinch(selector, state);

	// When grabbing this tool we change ownership to the selector which is owned by the player pawn.
	// While ownership does not give authority to change replicated properties it does allow the player to call server functions on the tool.
	if (state && !GetOwner()->HasAuthority()) {

		if (!playerState)
		{
			APlayerState* PlayerStateAtIndex0 = UGameplayStatics::GetPlayerState(GetWorld(), 0);
			playerState = Cast<ABrickSpacePlayerState>(PlayerStateAtIndex0);
		}

		// We change ownership through the player state which is already owned by the player controller and can call server functions.
		playerState->Server_Own(GetOwner(), selector->GetOwner());
	}
}

void UTool::BeginPlay()
{
	Super::BeginPlay();

	//selectionFilter = 0x01;

	UStaticMeshComponent* mesh = Cast<UStaticMeshComponent>(clientComponent);
	if (mesh != nullptr)
	{
		// Add overlap with bricks.
		mesh->OnComponentBeginOverlap.AddDynamic(this, &UTool::OnOverlapBegin);
	}
}

void UTool::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// OverlappedComp is our local clientComponent
	// Search OtherActor for Brick components
	for (int i = 0; i < OtherComp->GetNumChildrenComponents(); i++) {
		brickTouched = Cast<UBrick>(OtherComp->GetChildComponent(i));
		if (brickTouched != nullptr)
		{
			ChangeBrickTouched();
			UE_LOG(LogTemp, Warning, TEXT("OnOverlapBegin brickTouched:%s"), *FString(brickTouched->ClientName()));
			break;
		}
	}
}

// Must override in inheriting classes.
void UTool::ChangeBrickTouched() {}
