#pragma once

#include "CoreMinimal.h"
#include "Vodget.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "VodgetPanel.generated.h"

UCLASS(ClassGroup = (UI), meta = (BlueprintSpawnableComponent))
class BRICKSPACE_API UVodgetPanel : public UVodget
{
	GENERATED_BODY()

public:
	UVodgetPanel();

	// Simple size control (width x height in cm)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Panel", meta = (ClampMin = "5", ClampMax = "50"))
	float Width = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Panel", meta = (ClampMin = "5", ClampMax = "50"))
	float Height = 15.0f;

	// Basic visual properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Panel")
	FLinearColor PanelColor = FLinearColor(0.1f, 0.1f, 0.2f, 0.8f);

	// Panel mesh settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Panel")
	UStaticMesh* PanelMeshAsset = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Panel")
	UMaterialInterface* PanelMaterial = nullptr;

	// Show in editor for positioning (game uses palm detection)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Panel")
	bool bShowInEditor = true;

	// Auto-hide when hand faces away
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Panel")
	bool bAutoHide = true;

	UFUNCTION(BlueprintCallable, Category = "Panel")
	void SetPanelColor(FLinearColor NewColor);

	UFUNCTION(BlueprintCallable, Category = "Panel")
	void ShowPanel(bool bShow = true);

	UFUNCTION(BlueprintCallable, Category = "Panel")
	void HidePanel() { ShowPanel(false); }

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void OnRegister() override;
#endif

	// Panel mesh created during construction
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Panel")
	UStaticMeshComponent* PanelMesh = nullptr;

private:
	UPROPERTY()
	UMaterialInstanceDynamic* DynMaterial = nullptr;

	bool bLastPalmUp = true;

	void UpdatePanelVisuals();
	bool IsPalmFacingUser() const;
	void SetPanelAndChildrenVisibility(bool bIsVisible);
};
