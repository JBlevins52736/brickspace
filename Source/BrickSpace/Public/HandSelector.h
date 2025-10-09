// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Selector.h"
class ABrickSpacePawn;
class UOculusXRHandComponent;
struct FXRMotionControllerData;
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
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VAR")
	UOculusXRHandComponent* skRef = nullptr; // skRef is the component I need to get bone positions because for some reason this needs to exist

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




	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VAR")
	UChildActorComponent* menuSubsystemActor = nullptr;



protected:
	// Called when the game starts
	virtual void SetCursor() override;

	virtual void GrabFocus(bool val) override;

private:
	UVodget* DoRaycast();
	void CalculateHandSize();
	// Handles all gesture commands
	void CheckHandGestures(float deltaTime);
	void UpdatePalmTrackingPoint();
	// Handles grabbing objects
	void HandGrabGesture(const FVector& palmPos);
	void WorldGrabGesture(const FVector& palmPos);
	void CalculateEyeHandPosBoneData(FVector& startVector, FVector& endPos);
	inline FVector GetMidPointBetweenThumbIndex();
	void DetectActivationMenuSystem();
	float ratioHitEyeOverHandEye = 0.0;
	float relativeHandSizeSquared = 0;
	// The hit result gets populated by the line trace
	FHitResult Hit;
	TArray<FName> boneNames; // references my hands
	FName palmName = FName("Wrist Root"); // need these to reference my hands
	FVector palmPreviousState = FVector::Zero();
	FVector handTravelDirection;
	bool handTrackingActive = false;
	bool isUsingWorldGrabber = false;
	bool palmInMotion = false;
	float timeControlMenuButtonPresses = 0.0f;
	float squaredHandToEyeDistance = 0.0f;
	
};
