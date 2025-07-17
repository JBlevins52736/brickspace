#include "VodgetPanel.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "Materials/Material.h"

UVodgetPanel::UVodgetPanel()
{
	PrimaryComponentTick.bCanEverTick = true;

	// Create panel mesh during construction (safe)
	PanelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PanelMesh"));
	if (PanelMesh)
	{
		// Load default plane mesh
		UStaticMesh* DefaultPlane = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Plane.Plane"));
		if (DefaultPlane)
		{
			PanelMesh->SetStaticMesh(DefaultPlane);
		}

		// Set collision for raycast detection
		PanelMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		PanelMesh->SetCollisionResponseToAllChannels(ECR_Block);

		// Scale to desired size
		PanelMesh->SetRelativeScale3D(FVector(Width / 100.0f, Height / 100.0f, 1.0f));
		PanelMesh->SetRelativeRotation(FRotator(0, 0, 90.0f)); // Face forward
	}
}

void UVodgetPanel::BeginPlay()
{
	Super::BeginPlay();
	UpdatePanelVisuals();

	// Initial palm check and set visibility for panel and children
	if (bAutoHide)
	{
		bLastPalmUp = IsPalmFacingUser();
		SetPanelAndChildrenVisibility(bLastPalmUp);
	}
}

void UVodgetPanel::UpdatePanelVisuals()
{
	if (!PanelMesh) return;

	// Update mesh if custom asset is provided
	if (PanelMeshAsset)
	{
		PanelMesh->SetStaticMesh(PanelMeshAsset);
	}

	// Scale the panel to desired size (default plane is 100x100 units)
	FVector PanelScale(Width / 100.0f, Height / 100.0f, 1.0f);
	PanelMesh->SetRelativeScale3D(PanelScale);
	PanelMesh->SetRelativeRotation(FRotator(0, 0, 90.0f)); // Face forward

	// Apply material - use custom material if provided, otherwise create basic one
	if (PanelMaterial)
	{
		// Use the assigned material
		PanelMesh->SetMaterial(0, PanelMaterial);

		// Try to create dynamic material instance for color control
		DynMaterial = PanelMesh->CreateAndSetMaterialInstanceDynamic(0);
	}
	else
	{
		// Create a basic translucent material that supports color and opacity
		UMaterialInterface* BaseMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
		if (BaseMaterial)
		{
			PanelMesh->SetMaterial(0, BaseMaterial);
			DynMaterial = PanelMesh->CreateAndSetMaterialInstanceDynamic(0);
		}
	}

	// Update material color and opacity
	if (DynMaterial)
	{
		// Try multiple parameter names to ensure color shows up
		DynMaterial->SetVectorParameterValue("Color", PanelColor);
		DynMaterial->SetVectorParameterValue("BaseColor", PanelColor);
		DynMaterial->SetVectorParameterValue("Albedo", PanelColor);
		DynMaterial->SetVectorParameterValue("Tint", PanelColor);

		// Set opacity/alpha
		DynMaterial->SetScalarParameterValue("Opacity", PanelColor.A);
		DynMaterial->SetScalarParameterValue("Alpha", PanelColor.A);
	}

	// Handle editor vs game visibility
	bool bIsInGame = GetWorld() && GetWorld()->IsGameWorld();
	if (!bIsInGame)
	{
		// In editor - respect bShowInEditor setting
		PanelMesh->SetVisibility(bShowInEditor);
	}
	// In game - visibility controlled by palm detection
}

bool UVodgetPanel::IsPalmFacingUser() const
{
	const APlayerCameraManager* Cam = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	if (!Cam) return true;

	// Get palm up vector (Z-axis of component)
	FVector PalmUp = GetComponentTransform().GetUnitAxis(EAxis::Z);
	FVector ToCamera = (Cam->GetCameraLocation() - GetComponentLocation()).GetSafeNormal();

	// Return true if palm is facing toward camera
	return FVector::DotProduct(PalmUp, ToCamera) > -0.3f;
}

void UVodgetPanel::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bAutoHide)
	{
		bool bPalmUp = IsPalmFacingUser();
		if (bPalmUp != bLastPalmUp)
		{
			bLastPalmUp = bPalmUp;
			SetPanelAndChildrenVisibility(bPalmUp);
		}
	}
}

void UVodgetPanel::SetPanelColor(FLinearColor NewColor)
{
	PanelColor = NewColor;
	UpdatePanelVisuals();
}

void UVodgetPanel::ShowPanel(bool bShow)
{
	SetPanelAndChildrenVisibility(bShow);
}

void UVodgetPanel::SetPanelAndChildrenVisibility(bool bIsVisible)
{
	// Show/hide the panel mesh only when palm is visible (no manual override)
	if (PanelMesh)
	{
		PanelMesh->SetVisibility(bIsVisible);
	}

	// Hide/show all child Static Mesh Components (buttons, etc.)
	TArray<USceneComponent*> ChildComponents;
	GetChildrenComponents(true, ChildComponents); // true = include all descendants

	for (USceneComponent* Child : ChildComponents)
	{
		if (UStaticMeshComponent* ChildMesh = Cast<UStaticMeshComponent>(Child))
		{
			ChildMesh->SetVisibility(bIsVisible);
		}
	}

	// Also disable/enable Vodget children when hidden to prevent interaction
	TArray<UVodget*> ChildVodgets;
	GetOwner()->GetComponents<UVodget>(ChildVodgets);

	for (UVodget* ChildVodget : ChildVodgets)
	{
		if (ChildVodget != this && ChildVodget->GetAttachParent() == this)
		{
			// Disable/enable tick for child vodgets when panel is hidden
			ChildVodget->SetComponentTickEnabled(bIsVisible);
		}
	}
}

#if WITH_EDITOR
void UVodgetPanel::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property)
	{
		FName PropertyName = PropertyChangedEvent.Property->GetFName();

		// Rebuild visuals when relevant properties change
		if (PropertyName == GET_MEMBER_NAME_CHECKED(UVodgetPanel, Width) ||
			PropertyName == GET_MEMBER_NAME_CHECKED(UVodgetPanel, Height) ||
			PropertyName == GET_MEMBER_NAME_CHECKED(UVodgetPanel, PanelColor) ||
			PropertyName == GET_MEMBER_NAME_CHECKED(UVodgetPanel, PanelMeshAsset) ||
			PropertyName == GET_MEMBER_NAME_CHECKED(UVodgetPanel, PanelMaterial) ||
			PropertyName == GET_MEMBER_NAME_CHECKED(UVodgetPanel, bShowInEditor))
		{
			UpdatePanelVisuals();
		}
	}
}

void UVodgetPanel::OnRegister()
{
	Super::OnRegister();

	// Setup in editor
	if (!GetWorld() || !GetWorld()->IsGameWorld())
	{
		UpdatePanelVisuals();
	}
}
#endif