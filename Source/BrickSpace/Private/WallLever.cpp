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

//void UWallLever::ForePinch(USelector* cursor, bool state)
//{
//	if (state) // Grab begin
//	{
//		if (cursor && clientComponent)
//		{
//			grabbingSelector = cursor;
//
//			// Get cursor's world position
//			FVector worldPos = cursor->Cursor().GetLocation();
//
//			// Transform to the lever's local space (using the component's transform)
//			FVector localPos = GetComponentTransform().InverseTransformPosition(worldPos);
//
//			// SAVE the initial vector in the rotational plane (Y and Z)
//			// The X-axis is the axis of rotation, so we zero out X.
//			grabvec = FVector(0.0f, localPos.Y, localPos.Z);
//
//			// Optionally save the current rotation if needed for clamping logic later, 
//			// but the FQuat logic handles the delta itself.
//			// initialRotation = LeverArm->GetRelativeRotation(); 
//
//			cursor->GrabFocus(true);
//		}
//	}
//	else // Grab end
//	{
//		if (grabbingSelector)
//		{
//			grabbingSelector->GrabFocus(false);
//			grabbingSelector = nullptr;
//		}
//	}
//}
//void UWallLever::UpdateLeverFromSelector(USelector* cursor)
//{
//	if (!cursor || !clientComponent) return; // Use clientComponent for rotation
//
//	// 1. Get current vector in the rotational plane (Y and Z)
//	FVector worldPos = cursor->Cursor().GetLocation();
//	FVector localPos = GetComponentTransform().InverseTransformPosition(worldPos);
//	FVector currVec(0.0f, localPos.Y, localPos.Z); // Assuming rotation is around the local X (Roll) axis
//
//	// 2. Calculate the delta rotation quaternion
//	FQuat deltaRot = FQuat::FindBetweenVectors(grabvec, currVec);
//
//	// 3. Apply the delta rotation to the lever's current rotation
//	FQuat newRot = deltaRot * clientComponent->GetRelativeRotation().Quaternion();
//
//	// 4. Apply the rotation to the component
//	clientComponent->SetRelativeRotation(newRot);
//
//	// 5. Clamping the Rotation (Uses MinPitch/MaxPitch)
//
//	// Convert the new rotation back to Euler angles
//	FRotator currentEuler = clientComponent->GetRelativeRotation();
//
//	// Clamp the Roll to the defined angle range (e.g., -75 to 75)
//	float clampedRoll = FMath::Clamp(currentEuler.Roll, MinPitch, MaxPitch);
//
//	// Re-apply the clamped roll while keeping other axes' rotation
//	FRotator finalRot = FRotator(currentEuler.Pitch, currentEuler.Yaw, clampedRoll);
//	clientComponent->SetRelativeRotation(finalRot);
//
//	// The final value is the clamped angle itself, used to control speed.
//	float finalValue = clampedRoll;
//
//	// 6. Broadcast the final angle value directly (This is your speed value)
//	OnLeverMoved.Broadcast(finalValue);
//	UE_LOG(LogTemp, Log, TEXT("Lever Speed Value (Angle): %f"), finalValue);
//
//	// 7. Reset the grabvec for the next tick for continuous, smooth movement
//	grabvec = currVec;
//}
//
//void UWallLever::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//	if (grabbingSelector)
//	{
//		UpdateLeverFromSelector(grabbingSelector);
//	}
//
//}

// In ForePinch, you save the initial grab vector relative to the lever's root. This is correct.
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
	// This gives the rotation needed to go from the grab point (grabvec) to the current point (currVec).
	FQuat deltaRot = FQuat::FindBetweenVectors(grabvec, currVec);

	// 3. Apply the delta rotation to the lever's CURRENT relative rotation.
	// FIX: Use deltaRot.Inverse() to reverse the direction of rotation.
	FQuat newRotQuat = deltaRot.Inverse() * clientComponent->GetRelativeRotation().Quaternion();
	//                  ^^^^^^^^^^^^^^^ <-- THE FIX

	// Convert to FRotator to work with Euler angles for clamping
	FRotator newRot = newRotQuat.Rotator();


	// 4. Clamping and Final Application (Focus on Roll (X-axis))
	float currentRoll = newRot.Roll;

	// Clamp the Roll angle using the defined Pitch limits (MinPitch/MaxPitch)
	float clampedRoll = FMath::Clamp(currentRoll, MinPitch, MaxPitch);

	// Re-apply the clamped roll while zeroing out Pitch and Yaw
	FRotator finalRot = FRotator(0.0f, 0.0f, clampedRoll);
	clientComponent->SetRelativeRotation(finalRot);


	// 5. Broadcast the final angle value 
	float TotalRange = MaxPitch - MinPitch;
	float NormalizedRotation = (clampedRoll - MinPitch) / TotalRange;

	OnLeverMoved.Broadcast(NormalizedRotation);
	UE_LOG(LogTemp, Log, TEXT("Lever Normalized Rotation: %f (Angle: %f)"), NormalizedRotation, clampedRoll);

	// 6. Reset the grabvec for the next tick for continuous, smooth movement
	grabvec = currVec;
}

void UWallLever::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	// Note: We move the Tick enable/disable logic to ForePinch for better control
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (grabbingSelector)
	{
		UpdateLeverFromSelector(grabbingSelector);
	}
}