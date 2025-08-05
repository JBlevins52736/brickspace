// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Anchor.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BRICKSPACE_API UAnchor : public UActorComponent
{
	GENERATED_BODY()
	

protected:

	virtual void BeginPlay() override;
	UFUNCTION(Server, Reliable)
	void Server_OrientToAnchor(const FVector& NewLocation, const FRotator& NewRotation);


	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ApplyAnchorTransform(const FVector& NewLocation, const FRotator& NewRotation);
public:
	
	UFUNCTION(BlueprintCallable, Category = "Anchor")
	void OrientToAnchor(AActor* AnchorActor);


private:

};
