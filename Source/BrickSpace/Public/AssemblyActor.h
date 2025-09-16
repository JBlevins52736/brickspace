// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
class UAssembly;
#include "AssemblyActor.generated.h"

UCLASS()
class BRICKSPACE_API AAssemblyActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAssemblyActor();

	//UFUNCTION(Server, Reliable)
	//void Server_TryAdvanceLayer();

	UFUNCTION(Server, Reliable)
	void Server_Move(const FTransform& transform);

	UAssembly* assembly;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
