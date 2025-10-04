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

	// Current cursor world position
	FVector worldPos = cursor->Cursor().GetLocation();

	// Convert to lever’s local space
	FVector localPos = GetComponentTransform().InverseTransformPosition(worldPos);

	// Compute Z delta relative to initial grab
	float deltaZ = localPos.Z - initialGrabZ;

	// Convert delta to angle
	float rotationDelta = deltaZ * Sensitivity;

	// Base on initial rotation’s Pitch (you can swap axis if needed)
	float targetAngle = FMath::Clamp(initialRotation.Pitch + rotationDelta, MinPitch, MaxPitch);

	// Get current rotation
	FRotator currentRot = LeverArm->GetRelativeRotation();

	// Choose axis that matches your lever’s setup:
	FRotator targetRot = currentRot;
	/*targetRot.Pitch = targetAngle; */  // try this first
	 targetRot.Roll = targetAngle; // use this if Pitch is wrong
	// targetRot.Yaw  = targetAngle; // fallback if needed

	// Smoothly interpolate rotation
	FRotator smoothedRot = FMath::RInterpTo(
		currentRot,
		targetRot,
		GetWorld()->GetDeltaSeconds(),
		10.f // smoothing speed (tweak as needed)
	);

	// Apply smoothed rotation
	LeverArm->SetRelativeRotation(smoothedRot);

	// Broadcast normalized value (0–1)
	float normalized = (targetAngle - MinPitch) / (MaxPitch - MinPitch);
	LeverDelegate.Broadcast(normalized);
}

// Called every frame
void UWallLever::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//if (!grabbingSelector || !LeverArm) return;

	//// Get current cursor world position
	//FVector worldPos = grabbingSelector->Cursor().GetLocation();

	//// Convert to lever's local space
	//FVector localPos = GetComponentTransform().InverseTransformPosition(worldPos);

	//// Compute Z delta relative to initial grab
	//float deltaZ = localPos.Z - initialGrabZ;

	//// Convert delta to pitch angle
	//float rotationDelta = deltaZ * Sensitivity; // degrees

	//// Base on the initial rotation’s Pitch only
	//float targetPitch = initialRotation.Pitch + rotationDelta;

	//// Clamp to allowed range
	//targetPitch = FMath::Clamp(targetPitch, MinPitch, MaxPitch);

	//// Build a clean rotator where ONLY Pitch is used
	//FRotator newRot(0.f, 0.f, 0.f);
	//newRot.Pitch = targetPitch;

	//// Apply rotation relative to lever’s local space
	//LeverArm->SetRelativeRotation(newRot);

	//// Broadcast normalized value (0–1)
	//float normalized = (targetPitch - MinPitch) / (MaxPitch - MinPitch);
	//LeverDelegate.Broadcast(normalized);


}