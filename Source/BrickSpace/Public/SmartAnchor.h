#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/StaticMeshComponent.h"
#include "OculusXRAnchors.h"
#include "OculusXRAnchorBPFunctionLibrary.h"
#include "OculusXRSpatialAnchorComponent.h"
#include "SmartAnchor.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BRICKSPACE_API USmartAnchor : public UActorComponent
{
	GENERATED_BODY()

public:
	USmartAnchor();

	UFUNCTION(BlueprintCallable, Category = "SmartAnchor")
	void CreateAnchor(const bool Value);

	// Load all saved anchors
	UFUNCTION(BlueprintCallable, Category = "SmartAnchor")
	void LoadAnchors();

	// Clear all anchors
	UFUNCTION(BlueprintCallable, Category = "SmartAnchor")
	void ClearAllAnchors();

	UPROPERTY(BlueprintReadWrite, Category = "SmartAnchor")
	USceneComponent* Controller;
	UPROPERTY(BlueprintReadWrite)
	USceneComponent* Marker;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SmartAnchor")
	UStaticMesh* AnchorMesh;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//UOculusXRAnchorComponent* Anchor;
	bool isAddAnchor = false;

	FVector fromVec;
	FVector toVec;
	FVector dir; 


private:
	UPROPERTY()
	TArray<UOculusXRAnchorComponent*> SavedAnchors;
};