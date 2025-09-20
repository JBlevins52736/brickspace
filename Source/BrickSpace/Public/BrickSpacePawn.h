// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BrickSpacePlayerState.h"
#include "HandSelector.h"
#include "BrickSpacePawn.generated.h"

UCLASS()
class BRICKSPACE_API ABrickSpacePawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABrickSpacePawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Create the methods to broadcast the hand color change
	ABrickSpacePlayerState* playerState = nullptr;
	float elapsedTickTime = 0.0f;

public:	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VAR")
	UStaticMeshComponent* rightHand = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VAR")
	UStaticMeshComponent* leftHand = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VAR")
	float delayInterval = 0.0f;

//	void NotifyServerOfHandMatChange(USelector* selector, UMaterialInterface* material);

//	UFUNCTION(Server, Unreliable)
//	void ServerUpdatePlayerHandPos(AActor* target, FVector left, FVector right);

	//UFUNCTION(Server, Reliable)
	//void ServerUpdatePlayerHandColor(AActor* target, UMaterialInterface* color, USelector* selector);

	//UFUNCTION(Server, Unreliable)
	//void Server_MeshPosUpdate(AActor* target, USelector* selector, FVector pos);

	//void UpdateHandColor(UMaterialInterface* color, USelector* selector);
	//void UpdateHandPos(USelector* selector, FVector pos);

	void VARLog(FString methodName);

#ifdef BLAH
	// Called every frame
		virtual void Tick(float DeltaTime) override;
#endif

	//// Called to bind functionality to input
	//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
//
//private:
//	UHandSelector* leftSelector = nullptr;
//	UHandSelector* rightSelector = nullptr;
};
