#pragma once

#include "CoreMinimal.h"
#include "Vodget.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/UnrealType.h"

#include "VodgetPanel.generated.h"

UCLASS(ClassGroup = (UI), meta = (BlueprintSpawnableComponent))
class BRICKSPACE_API UVodgetPanel : public UVodget
{
	GENERATED_BODY()

public:
	UVodgetPanel();

	/* ---------- Visuals ---------- */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* PanelMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly) UStaticMesh* EdgeMesh   = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) UStaticMesh* CornerMesh = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly) FVector2D PanelSize = FVector2D(0.25f, 0.18f);
	UPROPERTY(EditAnywhere, BlueprintReadOnly) float BorderThickness = 0.01f;

	/* ---------- Palm logic ---------- */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Palm")
	bool bUsePalmLogic = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Palm")
	bool bAutoHideByPalmUp = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Palm")
	FVector LocalOffset = FVector(6.f, 0.f, -4.f);  // cm

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Palm",
	          meta = (ClampMin = "-1", ClampMax = "1"))
	float PalmDotThreshold = -0.30f;

	/** Public flag you can poll anywhere (no delegate) */
	UPROPERTY(BlueprintReadOnly, Category = "Palm")
	bool bIsPalmUp = true;

	/** Optional BP hook (override in a Blueprint child if desired) */
	UFUNCTION(BlueprintImplementableEvent, Category = "Palm")
	void PalmFacingChanged(bool NewFacingUp);

	/* ---------- Helpers ---------- */
	UFUNCTION(BlueprintCallable) void SetPanelTexture(UTexture2D* Tex);
	UFUNCTION(BlueprintCallable) void SetPanelOpacity(float Alpha);
	UFUNCTION(BlueprintCallable) virtual void RebuildPanel();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float, ELevelTick,
	                           FActorComponentTickFunction*) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& Prop) override;
#endif

	UPROPERTY() UStaticMeshComponent* Edge[4]{};
	UPROPERTY() UStaticMeshComponent* Corner[4]{};

private:
	UMaterialInstanceDynamic* Dyn = nullptr;

	void EnsureChildMeshes();
	void UpdateBorder();
	bool CalcPalmUp() const;
	void ApplyPalmLogic();
};
