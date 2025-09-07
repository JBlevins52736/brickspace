// Fill out your copyright notice in the Description page of Project Settings.


#include "BrickSpacePawn.h"
//#include <Kismet/GameplayStatics.h>
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
		playerState->Server_UpdatePlayerHandPos(this, this->GetActorTransform());
		elapsedTickTime -= delayInterval;
	}


}

// Called to bind functionality to input
void ABrickSpacePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}



