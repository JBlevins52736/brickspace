// Fill out your copyright notice in the Description page of Project Settings.


#include "BrickSpaceGameState.h"
#include "WorldGrabber.h"
#include <Kismet/GameplayStatics.h>
#include "Net/UnrealNetwork.h" // Required for DOREPLIFETIME

void ABrickSpaceGameState::OnRep_WorldScale()
{
	if (UWorld* World = GetWorld()) // Ensure a valid world context
	{	
		World->GetWorldSettings()->WorldToMeters = currWorldToMeters;

		if (APlayerController* LocalPlayerController = UGameplayStatics::GetPlayerController(World, 0))
		{
			UWorldGrabber* worldGrabber = Cast<UWorldGrabber>(LocalPlayerController->GetPawn()->GetRootComponent());
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