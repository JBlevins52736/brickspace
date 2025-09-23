// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BrickSpaceGameState.generated.h"

/**
 * 
 */
UCLASS()
class BRICKSPACE_API ABrickSpaceGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:

	UPROPERTY(ReplicatedUsing = OnRep_WorldScale)
	float currWorldToMeters = 100.0;

	UFUNCTION()
	virtual void OnRep_WorldScale();

};
