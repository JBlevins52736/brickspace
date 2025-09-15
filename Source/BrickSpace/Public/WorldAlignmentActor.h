// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WorldAlignmentActor.generated.h"

UCLASS()
class BRICKSPACE_API AWorldAlignmentActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWorldAlignmentActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FTransform endTransform = FTransform::Identity;
	FTransform startTransform = FTransform::Identity;
	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void SetAnchorPostions(FTransform anchorPostion);

};
