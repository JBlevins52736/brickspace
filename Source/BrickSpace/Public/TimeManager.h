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

	// Server RPCs will be on the Pawn, but these public wrappers remain.
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

	// The running state is replicated (server-authoritative)
	UPROPERTY(ReplicatedUsing = OnRep_Running)
	bool bIsRunning = false;

	UFUNCTION()
	virtual void OnRep_Running();

	// Local copy of elapsed time (maintained by server and clients)
	// We will *remove* the DOREPLIFETIME for this, making it local
	float ElapsedTime = 0.0f;

	void UpdateTextRenderer();

	// --- CLIENT RPCs (Called by Server to Client) ---
public: // <--- Place them here
	// Client RPCs (Called by Server to Client)
	UFUNCTION(Client, Reliable)
	void Client_StartTimer();

	UFUNCTION(Client, Reliable)
	void Client_StopTimer();

	UFUNCTION(Client, Reliable)
	void Client_ResetTimer();

	// Client-to-Server RPC (Called by Client to Server)
	UFUNCTION(Server, Reliable)
	void Server_SyncStoppedTime(float FinalTime);

private:
	UPROPERTY()
	UTextRenderComponent* TimerTextRenderer;

	// Server-only variable to store the final time when stopped
	float ServerStoppedTime = 0.0f;
};