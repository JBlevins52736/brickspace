#include "WallBrick.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"

static const FName TAG_SpawnWall(TEXT("spawn wall"));

void UWallBrick::BeginPlay()
{
    Super::BeginPlay();
    InitialTransform = clientComponent->GetComponentTransform();
}

void UWallBrick::ForePinch(USelector* selector, bool state)
{
    Super::ForePinch(selector, state);
    if(!bThresholdReached && !state)
    {
        clientComponent->SetWorldTransform(InitialTransform);
    }
}

void UWallBrick::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (bThresholdReached) return;
    if (!grabbingSelector) return;

    const float Dist = FVector::Dist(
        clientComponent->GetComponentLocation(),
        InitialTransform.GetLocation()
    );

    if (Dist <= DistanceThreshold)
    {
        //
        return;
    }

    OnThresholdReached();
}

void UWallBrick::OnThresholdReached()
{
    bThresholdReached = true;

    USceneComponent* SpawnWallParent = FindSpawnWallAncestor();
    if (SpawnWallParent)
    {
        SpawnReplacementAtHome(SpawnWallParent);
    }

    DecoupleFromParent();
}

void UWallBrick::SpawnReplacementAtHome(USceneComponent* AttachParentIfAny)
{
    if (!GetWorld() || !GetOwner()) return;

    AActor* Replacement = GetWorld()->SpawnActor<AActor>(
        GetOwner()->GetClass(),
        InitialTransform
    );

    if (Replacement && AttachParentIfAny)
    {
        if (USceneComponent* Root = Replacement->GetRootComponent())
        {
            Root->AttachToComponent(AttachParentIfAny, FAttachmentTransformRules::KeepWorldTransform);
        }
    }
}

void UWallBrick::DecoupleFromParent()
{
    clientComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

    if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(clientComponent))
    {
        Prim->SetMobility(EComponentMobility::Movable);
    }
}

USceneComponent* UWallBrick::FindSpawnWallAncestor() const
{
    USceneComponent* Cur = clientComponent;
    while (Cur)
    {
        if (Cur->ComponentHasTag("spawn wall"))
            return Cur;
        Cur = Cur->GetAttachParent();
    }
    return nullptr;
}
