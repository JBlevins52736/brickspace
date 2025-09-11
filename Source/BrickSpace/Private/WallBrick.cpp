#include "WallBrick.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Actor.h"
#include "BrickActor.h"
#include "Selector.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h" // Required for DOREPLIFETIME

#include "BrickSpacePlayerState.h"
#include <Kismet/GameplayStatics.h>

static const FName TAG_SpawnWall(TEXT("spawn wall"));

void UWallBrick::BeginPlay()
{
    Super::BeginPlay();
    //InitialTransform = clientComponent->GetComponentTransform();
    InitialTransform = clientComponent->GetOwner()->GetTransform();
}

void UWallBrick::ForePinch(USelector* selector, bool state)
{
    Super::ForePinch(selector, state);

    //if (state) {
    //    APlayerState* PlayerStateAtIndex0 = UGameplayStatics::GetPlayerState(GetWorld(), 0);
    //    ABrickSpacePlayerState* playerState = Cast<ABrickSpacePlayerState>(PlayerStateAtIndex0);

    //    playerState->Server_Own(GetOwner(), selector->GetOwner());
    //}

    if(!state && !bThresholdReached )
    {
        //clientComponent->SetWorldTransform(InitialTransform);
        ABrickActor* brickActor = Cast<ABrickActor>(GetOwner());
        brickActor->Server_Move(GetOwner(), InitialTransform);
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
    if (!clientComponent->GetOwner()) 
        return;

    APlayerState* PlayerStateAtIndex0 = UGameplayStatics::GetPlayerState(GetWorld(), 0);
    ABrickSpacePlayerState *playerState = Cast<ABrickSpacePlayerState>(PlayerStateAtIndex0);
    playerState->Server_CloneActor(GetOwner(), InitialTransform);

//    ABrickActor* brickActor = Cast<ABrickActor>(GetOwner()); 
//    brickActor->Server_Clone(brickActor, InitialTransform);

#ifdef BLAH
    USceneComponent* SpawnWallParent = FindSpawnWallAncestor();
    if (SpawnWallParent)
    {
        //SpawnReplacementAtHome(SpawnWallParent);
    }

    DecoupleFromParent();
#endif
}

void UWallBrick::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UWallBrick, bThresholdReached);
}

#ifdef BLAH
void UWallBrick::SpawnReplacement()
{
    if (!clientComponent->GetOwner()) return;

    playerState->Server_CloneActor(clientComponent->GetOwner(), InitialTransform );
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
    void UWallBrick::SpawnReplacementAtHome(USceneComponent * AttachParentIfAny)
    {
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
#endif

