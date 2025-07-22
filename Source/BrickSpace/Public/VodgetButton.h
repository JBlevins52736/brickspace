#pragma once
#include "CoreMinimal.h"
#include "Vodget.h"
#include "VodgetButton.generated.h"

// Define delegates for button events
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FButtonPressedDelegate, int32, ButtonID);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FButtonToggledDelegate, int32, ButtonID, bool, bIsOn);

/**
 * VodgetButton - Simple interactive button component for VR
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BRICKSPACE_API UVodgetButton : public UVodget
{
	GENERATED_BODY()

public:
	virtual void Focus(USelector* cursor, bool state) override;
	virtual void ForePinch(USelector* selector, bool state) override;

	UPROPERTY(BlueprintAssignable, Category = "Button")
	FButtonPressedDelegate OnPressed;

	UPROPERTY(BlueprintAssignable, Category = "Button")
	FButtonToggledDelegate OnToggled;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button")
	int32 ButtonID = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button")
	bool bIsToggle = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button")
	bool bIsOn = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button")
	bool bIsPressed = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button")
	FString ShortName;
};