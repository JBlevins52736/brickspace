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

    SetListeningEnabled(true);
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

        OnItemSpawned.Broadcast(SpawnedActor, ItemName);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to spawn actor for item: %s"), *ItemName);
    }

    return SpawnedActor;
}
// ===== DELEGATE SYSTEM =====

void UVodgetSpawner::RequestSpawnAtLocation(const FString& ItemName, const FTransform& SpawnTransform)
{
    OnSpawnAtLocationRequested.Broadcast(ItemName, SpawnTransform);
}

void UVodgetSpawner::SetListeningEnabled(bool bEnabled)
{
    if (bEnabled)
    {
        OnSpawnAtLocationRequested.AddDynamic(this, &UVodgetSpawner::HandleSpawnAtLocationRequest);
    }
    else
    {
        OnSpawnAtLocationRequested.RemoveDynamic(this, &UVodgetSpawner::HandleSpawnAtLocationRequest);
    }
}

void UVodgetSpawner::HandleSpawnAtLocationRequest(const FString& ItemName, const FTransform& SpawnTransform)
{
    SpawnByNameAtTransform(ItemName, SpawnTransform);
}

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

//USceneComponent* UVodgetSpawner::GetCurrentSpawnPoint(bool bAdvance)
//{
//    if (SpawnPoints.Num() == 0)
//    {
//        return nullptr;
//    }
//
//    CurrentSpawnPointIndex = FMath::Clamp(CurrentSpawnPointIndex, 0, SpawnPoints.Num() - 1);
//
//    USceneComponent* CurrentPoint = SpawnPoints[CurrentSpawnPointIndex];
//
//    if (bAdvance && bAutoAdvanceSpawnPoint)
//    {
//        CurrentSpawnPointIndex = (CurrentSpawnPointIndex + 1) % SpawnPoints.Num();
//    }
//
//    return CurrentPoint;
//}
//
//FTransform UVodgetSpawner::GetCurrentSpawnTransform(bool bAdvance)
//{
//    USceneComponent* SpawnPoint = GetCurrentSpawnPoint(bAdvance);
//
//    if (SpawnPoint)
//    {
//        FTransform Transform = SpawnPoint->GetComponentTransform();
//        UE_LOG(LogTemp, Log, TEXT("VodgetSpawner: Using spawn point %s at %s"),
//            *SpawnPoint->GetName(),
//            *Transform.GetLocation().ToString());
//        return Transform;
//    }
//
//    FVector Fallback = GetComponentLocation() + (GetComponentTransform().GetUnitAxis(EAxis::X) * 30.0f);
//    UE_LOG(LogTemp, Warning, TEXT("VodgetSpawner: No spawn points available, using fallback"));
//    return FTransform(FRotator::ZeroRotator, Fallback, FVector(1.0f));
//}
//
//AActor* UVodgetSpawner::SpawnBrick(const FSpawnableData* Data, const FTransform& Transform)
//{
//    if (!Data || !Data->ActorBlueprint || !GetWorld()) return nullptr;
//
//    AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(Data->ActorBlueprint, Transform);
//    if (SpawnedActor)
//    {
//        //UE_LOG(LogTemp, Log, TEXT("VodgetSpawner: Spawned %s at %s"), *Data->ShortName, *Transform.GetLocation().ToString());
//
//        OnBrickSpawned.Broadcast(SpawnedActor, Data->ShortName);
//    }
//    return SpawnedActor;
//}
//
//AActor* UVodgetSpawner::SpawnByIndex(int32 Index)
//{
//    const FSpawnableData* Data = GetBrickDataByIndex(Index);
//    FTransform T = GetCurrentSpawnTransform(); 
//    return SpawnBrick(Data, T);
//}
//
//AActor* UVodgetSpawner::SpawnCurrent()
//{
//    return SpawnByIndex(CurrentIndex);
//}
//
//AActor* UVodgetSpawner::SpawnByName(const FString& ShortName)
//{
//    const FSpawnableData* Data = GetBrickDataByName(ShortName);
//    FTransform T = GetCurrentSpawnTransform(); 
//    return SpawnBrick(Data, T);
//}
//
//AActor* UVodgetSpawner::SpawnAtNamedPoint(FName PointName)
//{
//    const FSpawnableData* Data = GetBrickDataByIndex(CurrentIndex);
//    FTransform T = GetNamedSpawnTransform(PointName);
//    return SpawnBrick(Data, T);
//}
//
//AActor* UVodgetSpawner::SpawnAtTransform(const FTransform& Transform)
//{
//    const FSpawnableData* Data = GetBrickDataByIndex(CurrentIndex);
//    return SpawnBrick(Data, Transform);
//}
//
//AActor* UVodgetSpawner::SpawnByNameAtTransform(const FString& ShortName, const FTransform& Transform)
//{
//    const FSpawnableData* Data = GetBrickDataByName(ShortName);
//    return SpawnBrick(Data, Transform);
//}
//
//void UVodgetSpawner::SetCurrentSpawnPointIndex(int32 Index)
//{
//    if (Index >= 0 && Index < SpawnPoints.Num())
//    {
//        CurrentSpawnPointIndex = Index;
//    }
//}
//
//void UVodgetSpawner::NextSpawnPoint()
//{
//    if (SpawnPoints.Num() > 0)
//    {
//        CurrentSpawnPointIndex = (CurrentSpawnPointIndex + 1) % SpawnPoints.Num();
//    }
//}
//
//void UVodgetSpawner::PreviousSpawnPoint()
//{
//    if (SpawnPoints.Num() > 0)
//    {
//        CurrentSpawnPointIndex = (CurrentSpawnPointIndex - 1 + SpawnPoints.Num()) % SpawnPoints.Num();
//    }
//}
//
//int32 UVodgetSpawner::GetSpawnPointCount() const
//{
//    return SpawnPoints.Num();
//}
//
//FString UVodgetSpawner::GetCurrentSpawnPointName() const
//{
//    if (SpawnPoints.IsValidIndex(CurrentSpawnPointIndex))
//    {
//        return SpawnPoints[CurrentSpawnPointIndex]->GetName();
//    }
//    return "Invalid";
//}
//
//// Delegate-based spawning functions
//void UVodgetSpawner::RequestSpawnByName(const FString& ShortName)
//{
//    OnSpawnByNameRequested.Broadcast(ShortName);
//}
//
//void UVodgetSpawner::SetListeningEnabled(bool bListeningEnabled)
//{
//    if (bListeningEnabled)
//    {
//        OnSpawnByNameRequested.AddDynamic(this, &UVodgetSpawner::HandleSpawnRequest);
//      //  UE_LOG(LogTemp, Log, TEXT("VodgetSpawner: Listening enabled"));
//    }
//    else
//    {
//        OnSpawnByNameRequested.RemoveDynamic(this, &UVodgetSpawner::HandleSpawnRequest);
//        //UE_LOG(LogTemp, Log, TEXT("VodgetSpawner: Listening disabled"));
//    }
//}
//
//void UVodgetSpawner::HandleSpawnRequest(const FString& ShortName)
//{
//    //UE_LOG(LogTemp, Log, TEXT("VodgetSpawner: Handling spawn request for %s"), *ShortName);
//    SpawnByName(ShortName);
//}
//
//void UVodgetSpawner::SetCurrentIndex(int32 Index)
//{
//    if (IsValidIndex(Index))
//    {
//        CurrentIndex = Index;
//    }
//}
//
//void UVodgetSpawner::NextBrick()
//{
//    if (CachedRowNames.Num() > 0)
//    {
//        CurrentIndex = (CurrentIndex + 1) % CachedRowNames.Num();
//    }
//}
//
//void UVodgetSpawner::PreviousBrick()
//{
//    if (CachedRowNames.Num() > 0)
//    {
//        CurrentIndex = (CurrentIndex - 1 + CachedRowNames.Num()) % CachedRowNames.Num();
//    }
//}
//
//const FSpawnableData* UVodgetSpawner::GetBrickDataByIndex(int32 Index) const
//{
//    if (!IsValidIndex(Index) || !SpawnDataTable) return nullptr;
//    return SpawnDataTable->FindRow<FSpawnableData>(CachedRowNames[Index], TEXT("GetBrickDataByIndex"));
//}
//
//const FSpawnableData* UVodgetSpawner::GetBrickDataByName(const FString& ShortName) const
//{
//    if (!SpawnDataTable) return nullptr;
//    const FName* RowName = ShortNameToRowNameMap.Find(ShortName);
//    return RowName ? SpawnDataTable->FindRow<FSpawnableData>(*RowName, TEXT("GetBrickDataByName")) : nullptr;
//}
//
//TArray<FString> UVodgetSpawner::GetAllShortNames() const
//{
//    TArray<FString> Keys;
//    ShortNameToRowNameMap.GetKeys(Keys);
//    return Keys;
//}
//
//bool UVodgetSpawner::IsValidIndex(int32 Index) const
//{
//    return Index >= 0 && Index < CachedRowNames.Num();
//}
//
//void UVodgetSpawner::CacheRowNames()
//{
//    CachedRowNames.Empty();
//    if (SpawnDataTable)
//    {
//        CachedRowNames = SpawnDataTable->GetRowNames();
//        UE_LOG(LogTemp, Log, TEXT("VodgetSpawner: Cached %d bricks from data table"), CachedRowNames.Num());
//    }
//}
//
//void UVodgetSpawner::CacheShortNames()
//{
//    ShortNameToRowNameMap.Empty();
//    if (!SpawnDataTable) return;
//
//    for (const FName& RowName : CachedRowNames)
//    {
//        const FSpawnableData* Data = SpawnDataTable->FindRow<FSpawnableData>(RowName, TEXT("CacheShortNames"));
//        if (Data && !Data->ShortName.IsEmpty())
//        {
//            ShortNameToRowNameMap.Add(Data->ShortName, RowName);
//        }
//    }
//    UE_LOG(LogTemp, Log, TEXT("VodgetSpawner: Cached %d short names"), ShortNameToRowNameMap.Num());
//}
//
//FTransform UVodgetSpawner::GetNamedSpawnTransform(FName SlotName) const
//{
//    if (const USceneComponent* SpawnComp = NamedSpawnPoints.FindRef(SlotName))
//    {
//        return SpawnComp->GetComponentTransform();
//    }
//    FVector Fallback = GetComponentLocation() + (GetComponentTransform().GetUnitAxis(EAxis::X) * 30.0f);
//    UE_LOG(LogTemp, Warning, TEXT("VodgetSpawner: No spawn point for '%s'. Using fallback."), *SlotName.ToString());
//    return FTransform(FRotator::ZeroRotator, Fallback, FVector(1.0f));
//}
//
//FString UVodgetSpawner::GetCurrentBrickName() const
//{
//    const FSpawnableData* Data = GetBrickDataByIndex(CurrentIndex);
//    return Data ? Data->ShortName : FString("Invalid");
//}
