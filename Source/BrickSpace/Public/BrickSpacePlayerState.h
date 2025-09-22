// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Assembly.h"

#include "BrickSpacePlayerState.generated.h"

/**
 * 
 */
UCLASS()
class BRICKSPACE_API ABrickSpacePlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VAR")
	//AActor* groundplate;


};
