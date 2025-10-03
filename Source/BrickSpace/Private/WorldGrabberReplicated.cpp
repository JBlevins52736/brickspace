// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldGrabberReplicated.h"
#include "BrickSpacePawn.h"
#include "BrickSpaceGameState.h"
#include <Kismet/GameplayStatics.h>

void UWorldGrabberReplicated::BeginPlay()
{
	Super::BeginPlay();
	if (GetOwner()->HasAuthority())
		scaleMode = true;
}


void UWorldGrabberReplicated::OnRep_WorldScale(float worldScale)
{
	if (currWorldToMeters == worldScale)
		return;

	// The BrickSpaceGameState currWorldToMeters property was just changed by the listening server client.
	// Update our local copy of currWorldToMeters and scale the local and replicated hand meshes.
	SetWorldToMeters(worldScale);

	// Scale replicated hands via server so all clients are updated.
	if (leftHandReplicated && rightHandReplicated)
	{
		float handScale = currWorldToMeters / 100.0;
		FVector scaleVec = FVector::OneVector * handScale;

		ABrickSpacePawn* brickSpacePawn = Cast<ABrickSpacePawn>(GetOwner());
		if (brickSpacePawn->HasAuthority()) {
			// Scale replicated hands immediately when listen server client is updated. 
			leftHandReplicated->SetWorldScale3D(scaleVec);
			rightHandReplicated->SetWorldScale3D(scaleVec);
		}
		else {
			// Have server update replicated mesh scales.
			brickSpacePawn->Server_MeshScaleUpdate(leftHandReplicated, rightHandReplicated, handScale);
		}
	}
}


void UWorldGrabberReplicated::SetWorldToMeters(float worldScale)
{
	if (currWorldToMeters == worldScale)
		return;
	Super::SetWorldToMeters(worldScale);

	// Notify the GameState so all clients can be updated.
	if (GetOwner()->HasAuthority()) {
		ABrickSpaceGameState* GameState = Cast<ABrickSpaceGameState>(UGameplayStatics::GetGameState(GetWorld()));
		if (GameState)
		{
			GameState->currWorldToMeters = worldScale;
			GameState->OnRep_WorldScale();
		}
	}
}
