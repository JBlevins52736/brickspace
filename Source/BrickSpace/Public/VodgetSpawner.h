#include "CoreMinimal.h"
#include "Vodget.h"
#include "Engine/DataTable.h"
#include "VodgetSpawner.generated.h"

// Simple data structure for spawnable items
USTRUCT(BlueprintType)
struct FSpawnableData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawnable")
	TSubclassOf<AActor> ActorBlueprint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawnable")
	FText DisplayName;

	FSpawnableData()
	{
		ActorBlueprint = nullptr;
		DisplayName = FText::FromString("Unnamed Item");
	}
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BRICKSPACE_API UVodgetSpawner : public UVodget
{
	GENERATED_BODY()

public:
	UVodgetSpawner();

	// Data table reference
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
	UDataTable* SpawnDataTable = nullptr;

	// Current selected item index
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
	int32 CurrentIndex = 0;

	// Spawn distance in cm
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner", meta = (ClampMin = "10", ClampMax = "100"))
	float SpawnDistance = 30.0f;

	// Spawning functions
	UFUNCTION(BlueprintCallable, Category = "Spawner")
	AActor* SpawnByIndex(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "Spawner")
	AActor* SpawnCurrent();

	UFUNCTION(BlueprintCallable, Category = "Spawner")
	void SetCurrentIndex(int32 Index);

protected:
	virtual void BeginPlay() override;

private:
	TArray<FName> CachedRowNames;
	void CacheRowNames();
};