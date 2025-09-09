// Fill out your copyright notice in the Description page of Project Settings.


#include "BrickSpacePawn.h"
#include <Kismet/GameplayStatics.h>

#include "HandSelector.h"
#include "Net/UnrealNetwork.h"
// Sets default values
ABrickSpacePawn::ABrickSpacePawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	


}

// Called when the game starts or when spawned
void ABrickSpacePawn::BeginPlay()
{
	Super::BeginPlay();
	if (APlayerController* PC = Cast<APlayerController>(GetController())) {
		playerState = Cast<ABrickSpacePlayerState>(PC->PlayerState);
	}

}

void ABrickSpacePawn::NotifyServerOfHandMatChange(USelector* selector, UMaterialInterface* material)
{
	if (playerState)
	playerState->Server_ChangeHandColor(this, material);
	
}

// Called every frame
void ABrickSpacePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	elapsedTickTime += DeltaTime;

	if (elapsedTickTime > delayInterval && playerState) 
	{
		TArray<UActorComponent*> components;
		this->GetComponents<UActorComponent>(components);
		FTransform rightTrans;
		FTransform leftTrans;
		bool foundComp = false;
		for (UActorComponent* component : components)
		{
			if (component->GetName().Contains("HandSelectorL"))
			{
				UHandSelector* handSelector = Cast<UHandSelector>(component);
				leftTrans = handSelector->handMesh->GetComponentTransform();
				
			}
			else if (component->GetName().Contains("HandSelectorR"))
			{
				UHandSelector* handSelector = Cast<UHandSelector>(component);
				rightTrans = handSelector->handMesh->GetComponentTransform();
			}
		}
		playerState->Server_UpdatePlayerHandPos(this, leftTrans, rightTrans);
		elapsedTickTime -= delayInterval;
	}


}

// Called to bind functionality to input
void ABrickSpacePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}



