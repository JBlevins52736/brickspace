#pragma once

#include "CoreMinimal.h"
#include "Vodget.h"
#include <TimeManager.h>
#include "SliderButton.generated.h"


class USelector;
class ABrickSpacePawn;


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnButtonPressed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnButtonReleased);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BRICKSPACE_API USliderButton : public UVodget
{
	GENERATED_BODY()

public:
	USliderButton();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void Focus(USelector* cursor, bool state) override;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button")
	UStaticMeshComponent* StartButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button")
	UStaticMeshComponent* LaunchButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button")
	UStaticMeshComponent* ResetButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button")
	UStaticMeshComponent* PauseButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button")
	float MaxDepressionDistance = -5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button")
	float PressThreshold = -2.0f;


	UPROPERTY(BlueprintAssignable, Category = "Button")
	FOnButtonPressed OnPressed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer")
	UTimeManager* timer;

	UPROPERTY()
	bool isPressed = false;
	
	void Press();
private:

	float InitialLocalLocation;
	float InitialCursorOffsetZ = 0.0f;


	USelector* focusSelector = nullptr;


	



};