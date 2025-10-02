// Fill out your copyright notice in the Description page of Project Settings.


#include "BrickSpaceGameState.h"
#include "WorldGrabberReplicated.h"
#include <Kismet/GameplayStatics.h>
#include "Net/UnrealNetwork.h" // Required for DOREPLIFETIME

void ABrickSpaceGameState::OnRep_WorldScale()
{
	if (UWorld* World = GetWorld()) // Ensure a valid world context
	{	
		World->GetWorldSettings()->WorldToMeters = currWorldToMeters;

		if (APlayerController* LocalPlayerController = UGameplayStatics::GetPlayerController(World, 0))
		{
			UWorldGrabberReplicated* worldGrabber = Cast<UWorldGrabberReplicated>(LocalPlayerController->GetPawn()->GetRootComponent());
			if (worldGrabber)
				worldGrabber->OnRep_WorldScale( currWorldToMeters );	
		}
	}
}

void ABrickSpaceGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABrickSpaceGameState, currWorldToMeters);
}