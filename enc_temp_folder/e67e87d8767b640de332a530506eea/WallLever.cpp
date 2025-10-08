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


}

void UWallLever::BeginPlay()
{
	Super::BeginPlay();
	if (clientComponent)
	{
		CurrentLeverRotation = clientComponent->GetRelativeRotation();
		TargetLeverRotation = CurrentLeverRotation;
	}
}

void UWallLever::Focus(USelector* cursor, bool state)
{
	UVodget::Focus(cursor, state);

}

void UWallLever::ForePinch(USelector* cursor, bool state)
{
	if (state) // Grab begin
	{
		if (cursor && clientComponent)
		{
			// ... existing setup logic ...
			grabbingSelector = cursor;

			// Get cursor's world position
			FVector worldPos = cursor->Cursor().GetLocation();

			// Transform to the LEVER'S WORLD space (using the component's transform, which is the Lever component)
			// NOTE: clientComponent is the component you want to rotate (LeverArm or HandleSphere). 
			// We should transform worldPos relative to the *Lever's Owner* if the Lever is the root, 
			// but since you use GetComponentTransform() (the UWallLever component itself), we'll stick with that.
			FVector localPos = GetComponentTransform().InverseTransformPosition(worldPos);

			// SAVE the initial vector in the rotational plane (Y and Z). X is rotation axis.
			grabvec = FVector(0.0f, localPos.Y, localPos.Z);
			grabvec.Normalize(); // Normalize for cleaner FQuat::FindBetweenVectors

			cursor->GrabFocus(true);
			PrimaryComponentTick.SetTickFunctionEnable(true); // Ensure Tick starts on grab
		}
	}
	else // Grab end
	{
		if (grabbingSelector)
		{
			grabbingSelector->GrabFocus(false);
			grabbingSelector = nullptr;
			PrimaryComponentTick.SetTickFunctionEnable(false); // Ensure Tick stops on release
		}
	}
}
void UWallLever::UpdateLeverFromSelector(USelector* cursor)
{
	if (!cursor || !clientComponent) return;

	// 1. Get current vector in the rotational plane (Y and Z)
	FVector worldPos = cursor->Cursor().GetLocation();
	FVector localPos = GetComponentTransform().InverseTransformPosition(worldPos);
	FVector currVec(0.0f, localPos.Y, localPos.Z);
	currVec.Normalize();

	// 2. Calculate the delta rotation quaternion
	FQuat deltaRot = FQuat::FindBetweenVectors(grabvec, currVec);

	// 3. Calculate the RAW Target Rotation:
	// Use the TargetLeverRotation (which holds the desired, unsmoothed rotation state) 
	// as the base for calculating the new rotation.
	FQuat baseRotQuat = TargetLeverRotation.Quaternion();

	// Calculate the new target rotation by applying the inverse delta to the base rotation.
	// FQuat rawNewRotQuat = deltaRot.Inverse() * baseRotQuat; // This is for incremental, raw rotation.
	FQuat rawNewRotQuat = deltaRot.Inverse() * baseRotQuat;


	// --- STEPS 4 & 5: Clamping and Setting the SMOOTHING TARGET ---

	FRotator rawNewRot = rawNewRotQuat.Rotator();
	float currentRoll = rawNewRot.Roll;

	currentRoll *= Sensitivity;
	float clampedRoll = FMath::Clamp(currentRoll, MinPitch, MaxPitch);

	TargetLeverRotation = FRotator(0.0f, 0.0f, clampedRoll);

	// 6. Reset grabvec for continuous movement
	// IMPORTANT: This must remain *after* deltaRot is calculated and *before* the next tick.
	grabvec = currVec;
}


void UWallLever::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (grabbingSelector)
	{
		// 1. Calculate the raw rotation target
		UpdateLeverFromSelector(grabbingSelector);

		// 2. SMOOTH THE MOVEMENT
		// Use a high interp speed (e.g., 20.0) to smooth out the jitters.
		const float InterpSpeed = 20.0f;

		// FMath::RInterpTo moves the CurrentLeverRotation toward the TargetLeverRotation
		CurrentLeverRotation = FMath::RInterpTo(
			CurrentLeverRotation,
			TargetLeverRotation,
			DeltaTime,
			InterpSpeed
		);

		// 3. APPLY THE SMOOTHED ROTATION TO THE COMPONENT
		clientComponent->SetRelativeRotation(CurrentLeverRotation);

		// 4. BROADCAST THE SMOOTHED ANGLE
		float TotalRange = MaxPitch - MinPitch;
		float ClampedRoll = CurrentLeverRotation.Roll;
		float NormalizedRotation = (ClampedRoll - MinPitch) / TotalRange;

		OnLeverMoved.Broadcast(NormalizedRotation);
		UE_LOG(LogTemp, Log, TEXT("Lever SMOOTHED Rotation: %f (Angle: %f)"), NormalizedRotation, ClampedRoll);
	}
	// Optimization: When the lever is released and has reached its final position, turn off tick.
	else if (!TargetLeverRotation.Equals(CurrentLeverRotation, 0.01f))
	{
		// Still interpolating after release, finish the smoothing
		CurrentLeverRotation = FMath::RInterpTo(
			CurrentLeverRotation,
			TargetLeverRotation,
			DeltaTime,
			20.0f // Maintain speed
		);
		clientComponent->SetRelativeRotation(CurrentLeverRotation);
	}
	else
	{
		// Completely stopped and released, disable tick for max performance
		PrimaryComponentTick.SetTickFunctionEnable(false);
	}
}