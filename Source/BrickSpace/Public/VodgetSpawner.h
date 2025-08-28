#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Engine/DataTable.h"
#include "VodgetSpawner.generated.h"

// Simple scene data structures
USTRUCT(BlueprintType)
struct FSimpleObjectData {
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ObjectType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector Location = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FRotator Rotation = FRotator::ZeroRotator;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString MaterialName = TEXT("");

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsSolid = true;

    FSimpleObjectData() {}
    FSimpleObjectData(const FString& Type, const FTransform& Transform) {
        ObjectType = Type;
        Location = Transform.GetLocation();
        Rotation = Transform.GetRotation().Rotator();
    }
};

USTRUCT(BlueprintType)
struct FSimpleSceneData {
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FSimpleObjectData> Objects;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString SessionName = TEXT("Default");

    FSimpleSceneData() {}
};

// Spawnable data structure
USTRUCT(BlueprintType)
struct FSpawnableData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawnable")
    TSubclassOf<AActor> ActorBlueprint;
};

/**
 * Streamlined VodgetSpawner - only what you need + scene sync
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BRICKSPACE_API UVodgetSpawner : public USceneComponent
{
    GENERATED_BODY()

public:
    UVodgetSpawner();

    // ===== CORE CONFIGURATION =====
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configuration")
    UDataTable* SpawnDataTable = nullptr;

    // ===== CORE SPAWNING - What you actually use =====
    UFUNCTION(BlueprintCallable, Category = "Spawning")
    AActor* SpawnByNameAtTransform(const FString& ItemName, const FTransform& Transform);

    // ===== SCENE SYNC - New functionality =====
    /*UFUNCTION(BlueprintCallable, Category = "Scene Sync")
    void SaveSceneToJSON(const FString& Filename = "SceneState.json");

    UFUNCTION(BlueprintCallable, Category = "Scene Sync")
    void LoadSceneFromJSON(const FString& Filename = "SceneState.json");

    UFUNCTION(BlueprintCallable, Category = "Scene Sync")
    void SendSceneToClient(APlayerController* Client);*/

protected:
    virtual void BeginPlay() override;

private:
    // ===== CACHED DATA =====
    UPROPERTY()
    TArray<FName> CachedRowNames;

    UPROPERTY()
    FSimpleSceneData CurrentScene;

    // ===== CORE HELPERS =====
    const FSpawnableData* GetItemDataByName(const FString& ItemName) const;
    void CacheRowNames();

    // ===== SCENE SYNC HELPERS =====
  /*  FSimpleSceneData GatherCurrentScene();
    void SpawnFromSceneData(const FSimpleSceneData& SceneData);
    FString GetSceneFilePath(const FString& Filename) const;*/
};