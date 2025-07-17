#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

class UVodget;
class UMotionControllerComponent;

#include "Selector.generated.h"

UCLASS(Abstract, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BRICKSPACE_API USelector : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USelector();


	// When a selectors focus is grabbed(true) by a Vodget client (focusVodget), the Selector cannot focus
	// on another vodget client until GrabFocus(false) is called by focusVodget releasing the grab.
	virtual void GrabFocus(bool val)
	{
		focus_grabbed = val;
	}

	// Inheriting selectors must all maintain a model of a 3D cursor in world coordinates. 
	const FTransform& Cursor();

	virtual void SetFilter(uint16 filter);

	UPROPERTY(EditAnywhere, meta = (Bitmask), Category = "VAR")
	uint16 selectionFilter = 0xFF;

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "VAR", meta = (AllowPrivateAccess = true))
	USceneComponent* hand = nullptr;

	// Inheriting selectors must all override SetCursor() to set FTransform cursor based on the selection method. 
	virtual void SetCursor();
	FTransform cursor;

	// The focusVodget is the current selected vodget maintained by each inheriting selector.
	UVodget* focusVodget = nullptr;

	// Set when client vodgets call GrabFocus() above.
	bool focus_grabbed = false;

private:

	UFUNCTION(BlueprintCallable, Category = "VAR", meta = (AllowPrivateAccess = true))
	void ForePinch(const bool Value);

	UFUNCTION(BlueprintCallable, Category = "VAR", meta = (AllowPrivateAccess = true))
	void MiddlePinch(const bool Value);

	UFUNCTION(BlueprintCallable, Category = "VAR", meta = (AllowPrivateAccess = true))
	void Grip(const bool Value);

};
