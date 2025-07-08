#pragma once

#include "CoreMinimal.h"
#include "Vodget.h"
#include "VodgetButton.generated.h"

/* Forward declaration only ─ we never dereference the selector */
class USelector;

/* ── Events ─────────────────────────────────────────────────────────── */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnButtonPressed , int32, ButtonID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnButtonToggled, int32, ButtonID,
                                             bool , bIsNowOn);

/* ── Component ──────────────────────────────────────────────────────── */
UCLASS(ClassGroup=(UI), meta=(BlueprintSpawnableComponent))
class BRICKSPACE_API UVodgetButton : public UVodget
{
	GENERATED_BODY()

public:
	UVodgetButton();

	/* Designer options */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Button")
	int32 ButtonID = 0;

	/** Toggle (ON / OFF) or momentary Trigger */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Button")
	bool  bIsToggle = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Button|Motion")
	float PressDepth  = 2.f;     // cm travel

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Button|Motion")
	float SpringSpeed = 40.f;    // cm per second return speed

	/* Runtime state (read-only) */
	UPROPERTY(BlueprintReadOnly, Category="Button|State")
	bool bIsOn = false;          // only relevant when Toggle

	/* Signals */
	UPROPERTY(BlueprintAssignable, Category="Button|Event")
	FOnButtonPressed  OnButtonPressed;

	UPROPERTY(BlueprintAssignable, Category="Button|Event")
	FOnButtonToggled  OnButtonToggled;

	/* Vodget overrides */
	virtual void BeginPlay() override;
	virtual void ForePinch(USelector* cursor, bool bPinching) override;
	virtual void TickComponent(float DT, ELevelTick LT,
	                           FActorComponentTickFunction* TF) override;

private:
	/* Motion helpers */
	FVector HomeLoc;
	float   CurrAlpha   = 0.f;   // 0 = up, 1 = down
	float   TargetAlpha = 0.f;

	void   SetTarget(float NewA);
	void   UpdateTint();

	UMaterialInstanceDynamic* DynMat = nullptr;
};
