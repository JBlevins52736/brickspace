#pragma once
#include "CoreMinimal.h"
#include "SceneComponent.h"
#include "Engine/DataTable.h"
#include "VodgetSpawner.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSpawnAtLocationRequested, const FString&, ItemName, const FTransform&, SpawnTransform);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemSpawned, AActor*, SpawnedActor, const FString&, ItemName);


USTRUCT(BlueprintType)
struct FSpawnableData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawnable")
	TSubclassOf<AActor> ActorBlueprint;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BRICKSPACE_API UVodgetSpawner : public USceneComponent
{
	GENERATED_BODY()

public:
	UVodgetSpawner();

	// ===== EVENTS =====
	UPROPERTY(BlueprintAssignable, Category = "Spawner Events")
	FOnSpawnAtLocationRequested OnSpawnAtLocationRequested;

	UPROPERTY(BlueprintAssignable, Category = "Spawner Events")
	FOnItemSpawned OnItemSpawned;

	// ===== DATA CONFIGURATION =====
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
	UDataTable* SpawnDataTable = nullptr;

	// ===== CORE SPAWNING FUNCTION =====
	UFUNCTION(BlueprintCallable, Category = "Spawning")
	AActor* SpawnByNameAtTransform(const FString& ItemName, const FTransform& Transform);

	// ===== DELEGATE SYSTEM =====
	UFUNCTION(BlueprintCallable, Category = "Delegate System")
	void RequestSpawnAtLocation(const FString& ItemName, const FTransform& SpawnTransform);

	UFUNCTION(BlueprintCallable, Category = "Delegate System")
	void SetListeningEnabled(bool bEnabled = true);

protected:
	virtual void BeginPlay() override;

private:
	// ===== CACHED DATA =====
	TArray<FName> CachedRowNames;

	// ===== CORE LOGIC =====
	const FSpawnableData* GetItemDataByName(const FString& ItemName) const;

	// ===== DATA CACHING =====
	void CacheRowNames();

	// ===== DELEGATE HANDLERS =====
	UFUNCTION()
	void HandleSpawnAtLocationRequest(const FString& ItemName, const FTransform& SpawnTransform);
};

//USTRUCT(BlueprintType)
//struct FSpawnableData : public FTableRowBase
//{
//	GENERATED_BODY()
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawnable")
//	TSubclassOf<AActor> ActorBlueprint;
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawnable")
//	FString ShortName = "UnnamedBrick";
//};
//
//UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
//class BRICKSPACE_API UVodgetSpawner : public UVodget
//{
//	GENERATED_BODY()
//
//public:
//	UVodgetSpawner();
//
//	// Delegates for external triggering and notification
//	UPROPERTY(BlueprintAssignable, Category = "Spawner Events")
//	FOnSpawnByNameRequested OnSpawnByNameRequested;
//
//	UPROPERTY(BlueprintAssignable, Category = "Spawner Events")
//	FOnBrickSpawned OnBrickSpawned;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
//	UDataTable* SpawnDataTable = nullptr;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
//	int32 CurrentIndex = 0;
//
//	// Spawn point management properties
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
//	int32 CurrentSpawnPointIndex = 0;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
//	bool bAutoAdvanceSpawnPoint = true;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conveyor")
//	TMap<FName, USceneComponent*> NamedSpawnPoints;
//
//	// Original spawn functions
//	UFUNCTION(BlueprintCallable, Category = "Spawner")
//	AActor* SpawnByIndex(int32 Index);
//
//	UFUNCTION(BlueprintCallable, Category = "Spawner")
//	AActor* SpawnCurrent();
//
//	UFUNCTION(BlueprintCallable, Category = "Spawner")
//	AActor* SpawnByName(const FString& ShortName);
//
//	UFUNCTION(BlueprintCallable, Category = "Spawner")
//	AActor* SpawnAtNamedPoint(FName PointName);
//
//	UFUNCTION(BlueprintCallable, Category = "Spawner")
//	AActor* SpawnAtTransform(const FTransform& SpawnTransform);
//
//	UFUNCTION(BlueprintCallable, Category = "Spawner")
//	AActor* SpawnByNameAtTransform(const FString& ShortName, const FTransform& SpawnTransform);
//
//	// Spawn point control functions
//	UFUNCTION(BlueprintCallable, Category = "Spawner")
//	void SetCurrentSpawnPointIndex(int32 Index);
//
//	UFUNCTION(BlueprintCallable, Category = "Spawner")
//	void NextSpawnPoint();
//
//	UFUNCTION(BlueprintCallable, Category = "Spawner")
//	void PreviousSpawnPoint();
//
//	UFUNCTION(BlueprintCallable, Category = "Spawner")
//	int32 GetSpawnPointCount() const;
//
//	UFUNCTION(BlueprintCallable, Category = "Spawner")
//	FString GetCurrentSpawnPointName() const;
//
//	// Delegate-based spawning
//	UFUNCTION(BlueprintCallable, Category = "Spawner")
//	void RequestSpawnByName(const FString& ShortName);
//
//	UFUNCTION(BlueprintCallable, Category = "Spawner")
//	void SetListeningEnabled(bool bListeningEnabled = true);
//
//	// Original brick management functions
//	void SetCurrentIndex(int32 Index);
//	void NextBrick();
//	void PreviousBrick();
//	int32 GetBrickCount() const { return CachedRowNames.Num(); }
//	TArray<FString> GetAllShortNames() const;
//	FString GetCurrentBrickName() const;
//	bool IsValidIndex(int32 Index) const;
//
//protected:
//	virtual void BeginPlay() override;
//
//private:
//	// Original private members
//	TArray<FName> CachedRowNames;
//	TMap<FString, FName> ShortNameToRowNameMap;
//
//	// Spawn point array
//	TArray<USceneComponent*> SpawnPoints;
//
//	// Original private functions
//	AActor* SpawnBrick(const FSpawnableData* Data, const FTransform& Transform);
//	const FSpawnableData* GetBrickDataByIndex(int32 Index) const;
//	const FSpawnableData* GetBrickDataByName(const FString& ShortName) const;
//	void CacheRowNames();
//	void CacheShortNames();
//	FTransform GetNamedSpawnTransform(FName SlotName) const;
//
//	// Spawn point helper functions
//	USceneComponent* GetCurrentSpawnPoint(bool bAdvance = true);
//	FTransform GetCurrentSpawnTransform(bool bAdvance = true);
//
//	// Delegate handler
//	UFUNCTION()
//	void HandleSpawnRequest(const FString& ShortName);
//};
