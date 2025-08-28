#include "VodgetSpawner.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

UVodgetSpawner::UVodgetSpawner()
{
    PrimaryComponentTick.bCanEverTick = false;
  //  CurrentSpawnPointIndex = 0;
  //  bAutoAdvanceSpawnPoint = true;
}

void UVodgetSpawner::BeginPlay()
{
    Super::BeginPlay();

    CacheRowNames();
    //CacheShortNames();

   // SetListeningEnabled(true);
}
AActor* UVodgetSpawner::SpawnByNameAtTransform(const FString& ItemName, const FTransform& Transform)
{
    const FSpawnableData* Data = GetItemDataByName(ItemName);
    if (!Data || !Data->ActorBlueprint || !GetWorld())
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnByNameAtTransform failed: Data=%s, Blueprint=%s"),
            Data ? TEXT("Valid") : TEXT("Null"),
            (Data && Data->ActorBlueprint) ? TEXT("Valid") : TEXT("Null"));
        return nullptr;
    }

    // Debug the transform being used
    UE_LOG(LogTemp, Log, TEXT("Spawning %s at Location: %s, Rotation: %s"),
        *ItemName, *Transform.GetLocation().ToString(), *Transform.GetRotation().ToString());

    // Spawn with explicit transform and collision handling
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(Data->ActorBlueprint, Transform, SpawnParams);
    if (SpawnedActor)
    {
        // Verify the actor was spawned at correct location
        UE_LOG(LogTemp, Log, TEXT("Actor spawned successfully at: %s"), *SpawnedActor->GetActorLocation().ToString());

        //OnItemSpawned.Broadcast(SpawnedActor, ItemName);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to spawn actor for item: %s"), *ItemName);
    }

    return SpawnedActor;
}
// ===== DELEGATE SYSTEM =====


// ===== PRIVATE CORE LOGIC =====


const FSpawnableData* UVodgetSpawner::GetItemDataByName(const FString& ItemName) const
{
    if (!SpawnDataTable)
    {
        return nullptr;
    }

    FName RowName(*ItemName);
    return SpawnDataTable->FindRow<FSpawnableData>(RowName, TEXT("GetItemDataByName"));
}

// ===== PRIVATE DATA CACHING =====

void UVodgetSpawner::CacheRowNames()
{
    CachedRowNames.Empty();
    if (SpawnDataTable)
    {
        CachedRowNames = SpawnDataTable->GetRowNames();
        UE_LOG(LogTemp, Log, TEXT("VodgetSpawner: Cached %d items from data table"), CachedRowNames.Num());
    }
}
