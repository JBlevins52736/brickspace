#include "TestCube.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

ATestCube::ATestCube()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

    // Create and configure mesh
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CubeMesh"));
    RootComponent = Mesh;

    Mesh->SetIsReplicated(true);
    Mesh->SetMobility(EComponentMobility::Movable);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (MeshAsset.Succeeded())
    {
        Mesh->SetStaticMesh(MeshAsset.Object);
    }

    // Add UBrick component
    BrickComponent = CreateDefaultSubobject<UBrick>(TEXT("BrickComponent"));
    BrickComponent->SetupAttachment(Mesh); // optional: attach to mesh
}
