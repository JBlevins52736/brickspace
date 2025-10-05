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
	// Optional: highlight lever when focused
}

void UWallLever::ForePinch(USelector* cursor, bool state)
{
	if (state) // Begin grab
	{
		if (cursor)
		{
			grabbingSelector = cursor;

			// Use selector cursor transform
			FVector worldPos = cursor->Cursor().GetLocation();

			// Convert into local slider space
			FVector localPos = GetComponentTransform().InverseTransformPosition(worldPos);

			initialRotation = LeverArm->GetRelativeRotation();
			// Save initial grab value along Z
			initialGrabZ = localPos.Z;

			cursor->GrabFocus(true);

			// Remove highlight overlay when grabbed
			if (UMeshComponent* MeshComp = Cast<UMeshComponent>(clientComponent))
			{
				MeshComp->SetOverlayMaterial(nullptr);
			}
		}
	}
	else // End grab
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

	// Smooth lever rotation (same as before)
	FVector worldPos = cursor->Cursor().GetLocation();
	FVector localPos = GetComponentTransform().InverseTransformPosition(worldPos);
	float grabAxis = localPos.Z;
	float delta = grabAxis - initialGrabZ;
	float rotationDelta = -delta * Sensitivity; // Fixed the inversion
	float targetAngle = FMath::Clamp(initialRotation.Roll + rotationDelta, MinPitch, MaxPitch);

	FRotator currentRot = LeverArm->GetRelativeRotation();
	FRotator targetRot = currentRot;
	targetRot.Roll = targetAngle; // adjust axis
	FRotator smoothedRot = FMath::RInterpTo(currentRot, targetRot, GetWorld()->GetDeltaSeconds(), 10.f);
	LeverArm->SetRelativeRotation(smoothedRot);
	/*LeverDelegate.Broadcast(normalized);*/
	// *** FIX: Log the actual smoothed rotation's roll component for consistent values ***
	float normalized = smoothedRot.Roll; // Use the smoothed value for logging
	normalized = FMath::Clamp(normalized, MinPitch, MaxPitch);
	OnLeverMoved.Broadcast(normalized);
	UE_LOG(LogTemp, Log, TEXT("Rotation:%f"), normalized);

}


// Called every frame
void UWallLever::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	

	// Update lever rotation if being grabbed
	if (grabbingSelector)
	{
		UpdateLeverFromSelector(grabbingSelector);
	}

}