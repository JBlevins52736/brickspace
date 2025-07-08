// VodgetPanel.cpp
// -----------------------------------------------------------------------------
// Base 3-D panel scene-component with optional wrist-mount (“palm”) behaviour.
// Visuals: textured quad + 3-D border.
// Palm logic: offset, face-camera, auto-hide when palm faces away.
// -----------------------------------------------------------------------------

#include "VodgetPanel.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

// ──────────────────────────────────────────────────────────────────────────────
// Constructor
// ──────────────────────────────────────────────────────────────────────────────
UVodgetPanel::UVodgetPanel()
{
	PrimaryComponentTick.bCanEverTick = true;   // palm logic needs tick
}

// ──────────────────────────────────────────────────────────────────────────────
// BeginPlay
// ──────────────────────────────────────────────────────────────────────────────
void UVodgetPanel::BeginPlay()
{
	Super::BeginPlay();

	// Build quad + border
	RebuildPanel();

	// One-time wrist offset & rotation
	if (bUsePalmLogic)
	{
		SetRelativeLocation(LocalOffset);            // cm
		SetRelativeRotation(FRotator(0.f, 90.f, 0.f)); // face camera
	}

	// Initialise palm state & visibility
	ApplyPalmLogic();
}

// ──────────────────────────────────────────────────────────────────────────────
// Tick
// ──────────────────────────────────────────────────────────────────────────────
void UVodgetPanel::TickComponent(float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTick)
{
	Super::TickComponent(DeltaTime, TickType, ThisTick);

	if (bUsePalmLogic)
		ApplyPalmLogic();
}

// ──────────────────────────────────────────────────────────────────────────────
// Palm helpers
// ──────────────────────────────────────────────────────────────────────────────
bool UVodgetPanel::CalcPalmUp() const
{
	if (!GetAttachParent()) return true;   // fail-safe

	const FVector Up = GetAttachParent()->GetUpVector();
	const FVector CamFwd =
		UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetActorForwardVector();

	return FVector::DotProduct(Up, CamFwd) < PalmDotThreshold;
}

void UVodgetPanel::ApplyPalmLogic()
{
	const bool NewState = CalcPalmUp();

	// Auto-hide
	if (bAutoHideByPalmUp)
		SetVisibility(NewState, /*propagate*/true);

	// Notify BP override
	if (NewState != bIsPalmUp)
	{
		bIsPalmUp = NewState;
		PalmFacingChanged(NewState);       // BlueprintImplementableEvent
	}
}

// ──────────────────────────────────────────────────────────────────────────────
// Border & visual helpers
// ──────────────────────────────────────────────────────────────────────────────
void UVodgetPanel::EnsureChildMeshes()
{
	if (!PanelMesh)
		PanelMesh = Cast<UStaticMeshComponent>(clientComponent);

	for (int32 i = 0; i < 4; ++i)
	{
		if (!Edge[i])
		{
			const FName Name = *FString::Printf(TEXT("Edge_%d"), i);
			Edge[i] = NewObject<UStaticMeshComponent>(this, Name);
			Edge[i]->SetupAttachment(clientComponent);
			Edge[i]->RegisterComponent();
		}
		if (!Corner[i])
		{
			const FName Name = *FString::Printf(TEXT("Corner_%d"), i);
			Corner[i] = NewObject<UStaticMeshComponent>(this, Name);
			Corner[i]->SetupAttachment(clientComponent);
			Corner[i]->RegisterComponent();
		}
	}
}

void UVodgetPanel::UpdateBorder()
{
	if (!EdgeMesh) return;

	const float W = PanelSize.X * 50.f;         // half width  (cm)
	const float H = PanelSize.Y * 50.f;         // half height (cm)

	const FVector Pos[4] = { { 0,  H, 0}, { 0,-H, 0}, { -W,0,0}, { W,0,0} };
	const FRotator Rot[4] = { {0,90,0}, {0,90,0}, {0,0,0}, {0,0,0} };
	const float    Len[4] = { PanelSize.X, PanelSize.X, PanelSize.Y, PanelSize.Y };

	for (int32 i = 0; i < 4; ++i)
	{
		// Edge strip
		Edge[i]->SetStaticMesh(EdgeMesh);
		Edge[i]->SetRelativeLocation(Pos[i]);
		Edge[i]->SetRelativeRotation(Rot[i]);
		Edge[i]->SetRelativeScale3D(
			FVector(Len[i], BorderThickness, BorderThickness));

		// Optional corner cube/bevel
		if (CornerMesh)
		{
			const FVector C = Pos[i] + Rot[i].RotateVector(FVector(Len[i] * 50.f, 0, 0));
			Corner[i]->SetStaticMesh(CornerMesh);
			Corner[i]->SetRelativeLocation(C);
			Corner[i]->SetRelativeScale3D(FVector(BorderThickness * 100.f));
		}
	}
}

void UVodgetPanel::RebuildPanel()
{
	EnsureChildMeshes();

	if (PanelMesh)
		PanelMesh->SetRelativeScale3D(
			FVector(PanelSize.X, PanelSize.Y, 1.f));   // plane is 1×1 m

	if (!Dyn && PanelMesh)
		Dyn = PanelMesh->CreateDynamicMaterialInstance(0);

	UpdateBorder();
}

void UVodgetPanel::SetPanelTexture(UTexture2D* Tex)
{
	if (Dyn)
		Dyn->SetTextureParameterValue(TEXT("PanelTexture"), Tex);
}

void UVodgetPanel::SetPanelOpacity(float Alpha)
{
	if (Dyn)
		Dyn->SetScalarParameterValue(
			TEXT("PanelOpacity"), FMath::Clamp(Alpha, 0.f, 1.f));
}

// ──────────────────────────────────────────────────────────────────────────────
// Editor hot-update
// ──────────────────────────────────────────────────────────────────────────────
#if WITH_EDITOR
void UVodgetPanel::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	RebuildPanel();
}
#endif
