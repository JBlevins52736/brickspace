// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PainterActor.generated.h"

UCLASS()
class BRICKSPACE_API APainterActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APainterActor();

	UFUNCTION(Server, Reliable)
	void Server_Move(const FTransform& InitialTransform);

protected:
	//// Called when the game starts or when spawned
	//virtual void BeginPlay() override;

public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

};
