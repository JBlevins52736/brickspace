// Fill out your copyright notice in the Description page of Project Settings.


#include "WallLever.h"
#include "Components/SceneComponent.h"
#include "Selector.h"

//// Sets default values for this component's properties
UWallLever::UWallLever()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UWallLever::BeginPlay()
{
	Super::BeginPlay();

}

void UWallLever::Focus(USelector* cursor, bool state)
{
	UVodget::Focus(cursor, state);

}

void UWallLever::ForePinch(USelector* cursor, bool state)
{
	if (state) 
	{
		if (cursor)
		{
			grabbingSelector = cursor;

			FVector worldPos = cursor->Cursor().GetLocation();

			FVector localPos = GetComponentTransform().InverseTransformPosition(worldPos);

			initialRotation = LeverArm->GetRelativeRotation();
	
			initialGrabZ = localPos.Z;

			cursor->GrabFocus(true);

			if (UMeshComponent* MeshComp = Cast<UMeshComponent>(clientComponent))
			{
				MeshComp->SetOverlayMaterial(nullptr);
			}
		}
	}
	else 
	{
		if (grabbingSelector)
		{
			grabbingSelector->GrabFocus(false);
			grabbingSelector = nullptr;
		}
	}

}
void UWallLever::UpdateLeverFromSelector(USelector* cursor)
{
	if (!cursor || !LeverArm) return;


	FVector worldPos = cursor->Cursor().GetLocation();
	FVector localPos = GetComponentTransform().InverseTransformPosition(worldPos);
	float grabAxis = localPos.Z;
	float delta = grabAxis - initialGrabZ;
	float rotationDelta = -delta * Sensitivity; 
	float targetAngle = FMath::Clamp(initialRotation.Roll + rotationDelta, MinPitch, MaxPitch);

	FRotator currentRot = LeverArm->GetRelativeRotation();
	FRotator targetRot = currentRot;
	targetRot.Roll = targetAngle; 
	FRotator smoothedRot = FMath::RInterpTo(currentRot, targetRot, GetWorld()->GetDeltaSeconds(), 10.f);
	LeverArm->SetRelativeRotation(smoothedRot);

	float normalized = smoothedRot.Roll;
	normalized = FMath::Clamp(normalized, MinPitch, MaxPitch);
	OnLeverMoved.Broadcast(normalized);
	UE_LOG(LogTemp, Log, TEXT("Rotation:%f"), normalized);

}

void UWallLever::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (grabbingSelector)
	{
		UpdateLeverFromSelector(grabbingSelector);
	}

}