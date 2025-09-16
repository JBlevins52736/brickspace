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
	UFUNCTION()
	void OnRep_ChangeTransform();
	UPROPERTY(ReplicatedUsing = OnRep_ChangeTransform)
	FTransform endTransferTransform;

	UPROPERTY(ReplicatedUsing = OnRep_ChangeTransform)
	FTransform beginTransferTransform;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	
	FTransform endTransform = FTransform::Identity;
	
	FTransform startTransform = FTransform::Identity;
	

	UFUNCTION(Server, Reliable)
	void Server_UpdateClientTransforms(AActor* target, FTransform finalTransform, FTransform beginTransform);



public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void SetAnchorPostions(FTransform anchorPostion);

};
