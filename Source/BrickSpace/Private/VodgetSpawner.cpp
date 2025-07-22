#include "VodgetSpawner.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

UVodgetSpawner::UVodgetSpawner()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UVodgetSpawner::BeginPlay()
{
	Super::BeginPlay();

	for (USceneComponent* Child : GetAttachChildren())
	{
		if (Child->GetName() == "LeftFront")       NamedSpawnPoints.Add("LeftFront", Child);
		else if (Child->GetName() == "RightFront") NamedSpawnPoints.Add("RightFront", Child);
		else if (Child->GetName() == "LeftBack")    NamedSpawnPoints.Add("LeftBack", Child);
		else if (Child->GetName() == "RightBack")   NamedSpawnPoints.Add("RightBack", Child);
	}

	CacheRowNames();
	CacheShortNames();
}

AActor* UVodgetSpawner::SpawnBrick(const FSpawnableData* Data, const FTransform& Transform)
{
	if (!Data || !Data->ActorBlueprint || !GetWorld()) return nullptr;

	AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(Data->ActorBlueprint, Transform);
	if (SpawnedActor)
	{
		UE_LOG(LogTemp, Log, TEXT("VodgetSpawner: Spawned %s at %s"), *Data->ShortName, *Transform.GetLocation().ToString());
	}
	return SpawnedActor;
}

AActor* UVodgetSpawner::SpawnByIndex(int32 Index)
{
	const FSpawnableData* Data = GetBrickDataByIndex(Index);
	FTransform T = GetNamedSpawnTransform("LeftFront");
	return SpawnBrick(Data, T);
}

AActor* UVodgetSpawner::SpawnCurrent()
{
	return SpawnByIndex(CurrentIndex);
}

AActor* UVodgetSpawner::SpawnByName(const FString& ShortName)
{
	const FSpawnableData* Data = GetBrickDataByName(ShortName);
	FTransform T = GetNamedSpawnTransform("LeftFront");
	return SpawnBrick(Data, T);
}

AActor* UVodgetSpawner::SpawnAtNamedPoint(FName PointName)
{
	const FSpawnableData* Data = GetBrickDataByIndex(CurrentIndex);
	FTransform T = GetNamedSpawnTransform(PointName);
	return SpawnBrick(Data, T);
}

AActor* UVodgetSpawner::SpawnAtTransform(const FTransform& Transform)
{
	const FSpawnableData* Data = GetBrickDataByIndex(CurrentIndex);
	return SpawnBrick(Data, Transform);
}

AActor* UVodgetSpawner::SpawnByNameAtTransform(const FString& ShortName, const FTransform& Transform)
{
	const FSpawnableData* Data = GetBrickDataByName(ShortName);
	return SpawnBrick(Data, Transform);
}

void UVodgetSpawner::SetCurrentIndex(int32 Index)
{
	if (IsValidIndex(Index))
	{
		CurrentIndex = Index;
	}
}

void UVodgetSpawner::NextBrick()
{
	if (CachedRowNames.Num() > 0)
	{
		CurrentIndex = (CurrentIndex + 1) % CachedRowNames.Num();
	}
}

void UVodgetSpawner::PreviousBrick()
{
	if (CachedRowNames.Num() > 0)
	{
		CurrentIndex = (CurrentIndex - 1 + CachedRowNames.Num()) % CachedRowNames.Num();
	}
}

const FSpawnableData* UVodgetSpawner::GetBrickDataByIndex(int32 Index) const
{
	if (!IsValidIndex(Index) || !SpawnDataTable) return nullptr;
	return SpawnDataTable->FindRow<FSpawnableData>(CachedRowNames[Index], TEXT("GetBrickDataByIndex"));
}

const FSpawnableData* UVodgetSpawner::GetBrickDataByName(const FString& ShortName) const
{
	if (!SpawnDataTable) return nullptr;
	const FName* RowName = ShortNameToRowNameMap.Find(ShortName);
	return RowName ? SpawnDataTable->FindRow<FSpawnableData>(*RowName, TEXT("GetBrickDataByName")) : nullptr;
}

TArray<FString> UVodgetSpawner::GetAllShortNames() const
{
	TArray<FString> Keys;
	ShortNameToRowNameMap.GetKeys(Keys);
	return Keys;
}

bool UVodgetSpawner::IsValidIndex(int32 Index) const
{
	return Index >= 0 && Index < CachedRowNames.Num();
}

void UVodgetSpawner::CacheRowNames()
{
	CachedRowNames.Empty();
	if (SpawnDataTable)
	{
		CachedRowNames = SpawnDataTable->GetRowNames();
		UE_LOG(LogTemp, Log, TEXT("VodgetSpawner: Cached %d bricks from data table"), CachedRowNames.Num());
	}
}

void UVodgetSpawner::CacheShortNames()
{
	ShortNameToRowNameMap.Empty();
	if (!SpawnDataTable) return;

	for (const FName& RowName : CachedRowNames)
	{
		const FSpawnableData* Data = SpawnDataTable->FindRow<FSpawnableData>(RowName, TEXT("CacheShortNames"));
		if (Data && !Data->ShortName.IsEmpty())
		{
			ShortNameToRowNameMap.Add(Data->ShortName, RowName);
		}
	}
	UE_LOG(LogTemp, Log, TEXT("VodgetSpawner: Cached %d short names"), ShortNameToRowNameMap.Num());
}

FTransform UVodgetSpawner::GetNamedSpawnTransform(FName SlotName) const
{
	if (const USceneComponent* SpawnComp = NamedSpawnPoints.FindRef(SlotName))
	{
		return SpawnComp->GetComponentTransform();
	}
	FVector Fallback = GetComponentLocation() + (GetComponentTransform().GetUnitAxis(EAxis::X) * 30.0f);
	UE_LOG(LogTemp, Warning, TEXT("VodgetSpawner: No spawn point for '%s'. Using fallback."), *SlotName.ToString());
	return FTransform(FRotator::ZeroRotator, Fallback, FVector(1.0f));
}

FString UVodgetSpawner::GetCurrentBrickName() const
{
	const FSpawnableData* Data = GetBrickDataByIndex(CurrentIndex);
	return Data ? Data->ShortName : FString("Invalid");
}
