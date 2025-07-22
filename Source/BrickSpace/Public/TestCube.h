#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Brick.h" // Make sure this points to your actual UBrick header
#include "TestCube.generated.h"

UCLASS()
class BRICKSPACE_API ATestCube : public AActor
{
    GENERATED_BODY()

public:
    ATestCube();

protected:
    UPROPERTY(VisibleAnywhere, Category = "Components")
    UStaticMeshComponent* Mesh;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UBrick* BrickComponent;
};
