// Fill out your copyright notice in the Description page of Project Settings.


#include "BrickSpacePawn.h"

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
	
}

// Called every frame
void ABrickSpacePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABrickSpacePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

