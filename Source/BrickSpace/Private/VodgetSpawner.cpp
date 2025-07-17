#include "VodgetSpawner.h"
#include "Engine/World.h"

UVodgetSpawner::UVodgetSpawner()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UVodgetSpawner::BeginPlay()
{
	Super::BeginPlay();
	CacheRowNames();
}

AActor* UVodgetSpawner::SpawnByIndex(int32 Index)
{
	if (!SpawnDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("VodgetSpawner: No SpawnDataTable assigned!"));
		return nullptr;
	}

	if (Index < 0 || Index >= CachedRowNames.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("VodgetSpawner: Invalid index %d"), Index);
		return nullptr;
	}

	FName RowName = CachedRowNames[Index];
	FSpawnableData* Data = SpawnDataTable->FindRow<FSpawnableData>(RowName, TEXT("SpawnByIndex"));

	if (!Data || !Data->ActorBlueprint)
	{
		UE_LOG(LogTemp, Warning, TEXT("VodgetSpawner: No blueprint found for index %d"), Index);
		return nullptr;
	}

	// Calculate spawn location
	FVector SpawnLocation = GetComponentLocation() + (GetComponentTransform().GetUnitAxis(EAxis::X) * SpawnDistance);
	FTransform SpawnTransform(FRotator::ZeroRotator, SpawnLocation, FVector(1.0f));

	// Spawn the actor in world space
	UWorld* World = GetWorld();
	if (World)
	{
		AActor* SpawnedActor = World->SpawnActor<AActor>(Data->ActorBlueprint, SpawnTransform);
		if (SpawnedActor)
		{
			UE_LOG(LogTemp, Log, TEXT("VodgetSpawner: Spawned %s"), *Data->DisplayName.ToString());
		}
		return SpawnedActor;
	}

	return nullptr;
}

AActor* UVodgetSpawner::SpawnCurrent()
{
	return SpawnByIndex(CurrentIndex);
}

void UVodgetSpawner::SetCurrentIndex(int32 Index)
{
	if (Index >= 0 && Index < CachedRowNames.Num())
	{
		CurrentIndex = Index;
	}
}

void UVodgetSpawner::CacheRowNames()
{
	CachedRowNames.Empty();

	if (SpawnDataTable)
	{
		CachedRowNames = SpawnDataTable->GetRowNames();
		UE_LOG(LogTemp, Log, TEXT("VodgetSpawner: Cached %d items from data table"), CachedRowNames.Num());
	}
}