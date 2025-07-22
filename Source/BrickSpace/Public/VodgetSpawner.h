#pragma once

#include "CoreMinimal.h"
#include "Vodget.h"
#include "Engine/DataTable.h"
#include "VodgetSpawner.generated.h"

USTRUCT(BlueprintType)
struct FSpawnableData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawnable")
	TSubclassOf<AActor> ActorBlueprint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawnable")
	FString ShortName = "UnnamedBrick";
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BRICKSPACE_API UVodgetSpawner : public UVodget
{
	GENERATED_BODY()

public:
	UVodgetSpawner();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
	UDataTable* SpawnDataTable = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
	int32 CurrentIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conveyor")
	TMap<FName, USceneComponent*> NamedSpawnPoints;

	UFUNCTION(BlueprintCallable, Category = "Spawner")
	AActor* SpawnByIndex(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "Spawner")
	AActor* SpawnCurrent();

	UFUNCTION(BlueprintCallable, Category = "Spawner")
	AActor* SpawnByName(const FString& ShortName);

	UFUNCTION(BlueprintCallable, Category = "Spawner")
	AActor* SpawnAtNamedPoint(FName PointName);

	UFUNCTION(BlueprintCallable, Category = "Spawner")
	AActor* SpawnAtTransform(const FTransform& SpawnTransform);

	UFUNCTION(BlueprintCallable, Category = "Spawner")
	AActor* SpawnByNameAtTransform(const FString& ShortName, const FTransform& SpawnTransform);

	void SetCurrentIndex(int32 Index);
	void NextBrick();
	void PreviousBrick();

	int32 GetBrickCount() const { return CachedRowNames.Num(); }
	TArray<FString> GetAllShortNames() const;
	FString GetCurrentBrickName() const;
	bool IsValidIndex(int32 Index) const;

protected:
	virtual void BeginPlay() override;

private:
	TArray<FName> CachedRowNames;
	TMap<FString, FName> ShortNameToRowNameMap;

	AActor* SpawnBrick(const FSpawnableData* Data, const FTransform& Transform);
	const FSpawnableData* GetBrickDataByIndex(int32 Index) const;
	const FSpawnableData* GetBrickDataByName(const FString& ShortName) const;
	void CacheRowNames();
	void CacheShortNames();
	FTransform GetNamedSpawnTransform(FName SlotName) const;
};