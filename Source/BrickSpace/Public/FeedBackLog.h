// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Vodget.h"
#include "Components/TextRenderComponent.h"

#include "FeedBackLog.generated.h"
/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BRICKSPACE_API UFeedBackLog : public UVodget
{
	GENERATED_BODY()
	
public:

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UFUNCTION(BlueprintCallable, Category = "VAR")
	void EnterAsync();

	UFUNCTION(BlueprintCallable, Category = "VAR")
	void ExitAsync(bool Success);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (Bitmask), Category = "VAR")
	UTextRenderComponent* log;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (Bitmask), Category = "VAR")
	UStaticMeshComponent* getMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (Bitmask), Category = "VAR")
	UStaticMeshComponent* feedbackUI;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (Bitmask), Category = "VAR")
	AActor* getMyFeed;

	void LogMessage(FString errorMsg);

	
protected:
	FText menuItemText;

	FLinearColor colorAsync;
	FLinearColor colorAsyncSuccess;
	FLinearColor colorAsyncFailure;
	float logVisiblitySecsRemaining;
	UMaterialInstanceDynamic* DynamicMaterialInstance;
};
