// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Selector.h"
class ABrickSpacePawn;
class FXRMotionControllerData;
#include "HandSelector.generated.h"

/**
 *
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BRICKSPACE_API UHandSelector : public USelector
{
	GENERATED_BODY()

public:

	UHandSelector();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VAR")
	USceneComponent* centerEye = nullptr;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void SetFilter(uint16 filter) override;

#pragma region HAND_MESH_POSITION_REPLICATION
	//UPROPERTY(ReplicatedUsing = OnRep_MeshPosUpdate)
	//FVector handPos;

	//UFUNCTION()
	//virtual void OnRep_MeshPosUpdate();

	UFUNCTION(Server, Unreliable)
	void Server_MeshPosUpdate(UHandSelector* selector, FVector pos);
#pragma endregion HAND_MESH_POSITION_REPLICATION


#pragma region HAND_MATERIAL_CHANGE_REPLICATION
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VAR")
	UStaticMeshComponent* handMesh = nullptr;

	virtual void SetMaterial(UMaterialInterface* color);

	UFUNCTION(Server, Reliable)
	void Server_SetMaterial(UMaterialInterface* color);

	UPROPERTY(ReplicatedUsing = OnRep_Material)
	UMaterialInterface* handMaterial;

	UFUNCTION()
	virtual void OnRep_Material();

	void VARLog(FString methodName);

#pragma endregion HAND_MATERIAL_CHANGE_REPLICATION

protected:
	// Called when the game starts
	virtual void SetCursor() override;

	virtual void GrabFocus(bool val) override;

private:
	UVodget* DoRaycast();

	float ratioHitEyeOverHandEye = 0.0;

	// The hit result gets populated by the line trace
	FHitResult Hit;

	bool handTrackingActive = false;

	// Handles all gesture commands
	void CheckHandGestures();

	// Handles grabbing objects
	void HandGrabGesture(FXRMotionControllerData &controllerData);
	
};
