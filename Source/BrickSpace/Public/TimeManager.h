#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TextRenderComponent.h"

class ABrickSpacePawn;

#include "TimeManager.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BRICKSPACE_API UTimeManager : public UActorComponent
{
	GENERATED_BODY()

public:

	UTimeManager();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	
	UFUNCTION(BlueprintCallable)
	void StartTimer(ABrickSpacePawn* Pawn);

	UFUNCTION(BlueprintCallable)
	void StopTimer(ABrickSpacePawn* pawn);

	UFUNCTION(BlueprintCallable)
	void ResetTimer(ABrickSpacePawn* pawn);

	UFUNCTION(BlueprintCallable)
	float GetElapsedTime() const;

	UFUNCTION(BlueprintCallable)
	void SetTextRenderer(UTextRenderComponent* InTextRenderer);

	
	UPROPERTY(ReplicatedUsing = OnRep_Running)
	bool bIsRunning = false;

	UFUNCTION()
	virtual void OnRep_Running();


	UPROPERTY(Replicated)
	float ElapsedTime = 0.0f;

	void UpdateTextRenderer();

	
public: 

	UFUNCTION(Server, Reliable)
	void Server_SyncStoppedTime(float FinalTime);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ResetTimer();
private:
	UPROPERTY()
	UTextRenderComponent* TimerTextRenderer;

	
	float ServerStoppedTime = 0.0f;
};