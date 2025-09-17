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
    InitialTransform = clientComponent->GetComponentTransform();
}

void UWallBrick::ForePinch(USelector* selector, bool state)
{
    //InitialTransform = clientComponent->GetComponentTransform();
    Super::ForePinch(selector, state);

    //if (state) {
    //    APlayerState* PlayerStateAtIndex0 = UGameplayStatics::GetPlayerState(GetWorld(), 0);
    //    ABrickSpacePlayerState* playerState = Cast<ABrickSpacePlayerState>(PlayerStateAtIndex0);

    //    playerState->Server_Own(GetOwner(), selector->GetOwner());
    //}

    if(!state && !bThresholdReached )
    {
        if ( !GetOwner()->HasAuthority() ) {

            if (!playerState) {
                APlayerState* PlayerStateAtIndex0 = UGameplayStatics::GetPlayerState(GetWorld(), 0);
                playerState = Cast<ABrickSpacePlayerState>(PlayerStateAtIndex0);
            }
            if (playerState)
            playerState->Server_Own(GetOwner(), selector->GetOwner());

            UE_LOG(LogTemp, Warning, TEXT("WallBrick lost authority to snap back?!") );
        }

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

    //APlayerState* PlayerStateAtIndex0 = UGameplayStatics::GetPlayerState(GetWorld(), 0);
    //ABrickSpacePlayerState *playerState = Cast<ABrickSpacePlayerState>(PlayerStateAtIndex0);
    //playerState->Server_CloneActor(GetOwner(), InitialTransform);

    //ABrickActor* brickActor = Cast<ABrickActor>(GetOwner()); 
    //brickActor->Server_Clone(InitialTransform);

    Server_Clone(InitialTransform);
}

void UWallBrick::Server_Clone_Implementation(const FTransform& onWallTransform)
{
    AActor* owner = GetOwner();
    AActor* clonedBrick = owner->GetWorld()->SpawnActor<AActor>(owner->GetClass(), onWallTransform);

    // When we set this from the server it will replicate to all clients.
    //UWallBrick* wallBrick = clonedBrick->FindComponentByClass<UWallBrick>();
    //if (wallBrick != nullptr)
    //    wallBrick->bThresholdReached = true;

    bThresholdReached = true;
}

void UWallBrick::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UWallBrick, bThresholdReached);
}
