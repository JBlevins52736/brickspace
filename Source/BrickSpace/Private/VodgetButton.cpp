#include "VodgetButton.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

UVodgetButton::UVodgetButton()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UVodgetButton::BeginPlay()
{
	Super::BeginPlay();

	HomeLoc = clientComponent->GetRelativeLocation();

	if (auto* SM = Cast<UStaticMeshComponent>(clientComponent))
		DynMat = SM->CreateAndSetMaterialInstanceDynamic(0);

	UpdateTint();
}

/* ── Input from selector ───────────────────────────────────────────── */
void UVodgetButton::ForePinch(USelector* /*cursor*/, bool bPinching)
{
	if (bPinching)                         // finger down
	{
		OnButtonPressed.Broadcast(ButtonID);

		if (bIsToggle)
		{
			bIsOn = !bIsOn;
			OnButtonToggled.Broadcast(ButtonID, bIsOn);
			SetTarget(bIsOn ? 1.f : 0.f);     // latch / unlatch
		}
		else
		{
			SetTarget(1.f);                   // momentary press
		}
	}
	else if (!bIsToggle)                    // finger up (trigger)
	{
		SetTarget(0.f);
	}
}

/* ── Motion & tint ─────────────────────────────────────────────────── */
void UVodgetButton::SetTarget(float NewA)
{
	TargetAlpha = FMath::Clamp(NewA, 0.f, 1.f);
	UpdateTint();
}

void UVodgetButton::TickComponent(float DT, ELevelTick LT,
	FActorComponentTickFunction* TF)
{
	Super::TickComponent(DT, LT, TF);

	if (FMath::IsNearlyEqual(CurrAlpha, TargetAlpha, 0.01f))
		return;

	CurrAlpha = FMath::FInterpTo(CurrAlpha, TargetAlpha,
		DT, SpringSpeed / PressDepth);

	const FVector NewLoc = HomeLoc + FVector(0, 0, -PressDepth * CurrAlpha);
	clientComponent->SetRelativeLocation(NewLoc);
}

void UVodgetButton::UpdateTint()
{
	if (!DynMat) return;

	const FLinearColor Up{ 0.14f,0.55f,1.f,1.f };
	const FLinearColor Down{ 0.04f,0.35f,0.9f,1.f };
	const FLinearColor On{ 0.03f,0.85f,0.25f,1.f };

	const FLinearColor Tint =
		(bIsToggle && bIsOn) ? On :
		(TargetAlpha > 0.5f) ? Down : Up;

	DynMat->SetVectorParameterValue(TEXT("Tint"), Tint);
}
