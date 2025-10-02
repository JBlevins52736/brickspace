#pragma once

#include "CoreMinimal.h"
#include "Vodget.h"
#include "TimeManager.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SphereComponent.h"   // if you expect spheres
#include "Components/BoxComponent.h"  
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
   /* virtual void Focus(USelector* cursor, bool state) override;*/
    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    // The mesh or component that moves when the button is pressed
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button")
    USceneComponent* StartButton;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button")
    USceneComponent* LaunchButton;

    // Collision triggers
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Button", meta = (AllowPrivateAccess = "true"))
    UBoxComponent* StartCollider;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Button", meta = (AllowPrivateAccess = "true"))
    UBoxComponent* LaunchCollider;
    //// Blueprint events
    //UPROPERTY(BlueprintAssignable, Category = "Button")
    //FOnButtonPressed OnPressed;

    //UPROPERTY(BlueprintAssignable, Category = "Button")
    //FOnButtonReleased OnReleased;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timer")
    UTimeManager* timer;
private:
    void Press();
    void Release();
};