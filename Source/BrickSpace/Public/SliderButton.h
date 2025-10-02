#pragma once

#include "CoreMinimal.h"
#include "Vodget.h"
#include <TimeManager.h>
#include "SliderButton.generated.h"


class USelector;
class ABrickSpacePawn;


// Delegates for Blueprint events
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

    // The mesh or component that moves when the button is pressed
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button")
    USceneComponent* StartButton;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button")
    USceneComponent* LaunchButton;
   
    // Blueprint events
    UPROPERTY(BlueprintAssignable, Category = "Button")
    FOnButtonPressed OnPressed;

    UPROPERTY(BlueprintAssignable, Category = "Button")
    FOnButtonReleased OnReleased;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer")
    UTimeManager* timer; 
    
    UPROPERTY()
    bool isPressed = false;
private:
    void Press();
    void Release();
    void DoRelease();

    FTimerHandle ReleaseDelayHandle;
    float ReleaseDelayDuration = 0.2f; // delay in seconds

};