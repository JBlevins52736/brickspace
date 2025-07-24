#pragma once

#include "CoreMinimal.h"
#include "Tool.h"
#include "BrickEreaser.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BRICKSPACE_API UBrickEreaser : public UTool
{
    GENERATED_BODY()

public:
    virtual void ForePinch(USelector* selector, bool state) override;

protected:
    virtual void BeginPlay() override;
};