#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TextRenderComponent.h"
#include "TimeManager.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BRICKSPACE_API UTimeManager : public UActorComponent
{
	GENERATED_BODY()

public:
	// Constructor
	UTimeManager();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Timer Control
	UFUNCTION(BlueprintCallable)
	void StartTimer();

	UFUNCTION(BlueprintCallable)
	void StopTimer();

	// Get elapsed time
	UFUNCTION(BlueprintCallable)
	float GetElapsedTime() const;

	// Set text renderer to update
	UFUNCTION(BlueprintCallable)
	void SetTextRenderer(UTextRenderComponent* InTextRenderer);

private:
	UPROPERTY()
	float ElapsedTime;

	UPROPERTY()
	bool bIsRunning;

	// The text renderer this component will update
	UPROPERTY()
	UTextRenderComponent* TimerTextRenderer;

	// Internal helper
	void UpdateTextRenderer();
};