// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tool.h"
#include "TestCube.h"
#include "Net/UnrealNetwork.h"
#include "RevealWand.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BRICKSPACE_API URevealWand : public UTool
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "VAR")
	UMaterialInterface* revealMaterial;

	URevealWand();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	UFUNCTION(Server, Reliable)
	void Server_RevealBrick(AActor* Target);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetBrickMaterial(AActor* TargetActor);

	

protected:
	virtual void BeginPlay() override;
	virtual void ChangeBrickTouched() override;
};
