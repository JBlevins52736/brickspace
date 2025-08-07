#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OculusXRSpatialAnchorComponent.h"
#include "OculusXRAnchorBPFunctionLibrary.h"
#include "SmartAnchor.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BRICKSPACE_API USmartAnchor : public UActorComponent
{
	GENERATED_BODY()

public:
	USmartAnchor();

	// Call this from a button to create an anchor
	UFUNCTION(BlueprintCallable, Category = "SmartAnchor")
	void CreateAnchor(const bool Value);
	UPROPERTY(BlueprintReadWrite)
	USceneComponent* Marker;
	UPROPERTY(BlueprintReadWrite)
	USceneComponent* controller;


protected:
	virtual void BeginPlay() override;

private:
	// Callback when anchor creation finishes
	UFUNCTION()
	void OnCreateAnchorComplete(EOculusXRAnchorResult::Type Result, UOculusXRAnchorComponent* Anchor);
};
