#pragma once

#include "CoreMinimal.h"
#include "Brick.h"
#include "WallBrick.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BRICKSPACE_API UWallBrick : public UBrick
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning", meta = (ClampMin = "0.0"))
    float DistanceThreshold = 100.0f;

    UPROPERTY(EditAnywhere, Replicated)
    bool bThresholdReached = false;  // only bool we need

	void Server_CloneWallBrick(const FTransform& onWallTransform);

protected:
    virtual void BeginPlay() override;
    virtual void ForePinch(USelector* selector, bool state) override;

    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    FTransform InitialTransform = FTransform::Identity;
};
