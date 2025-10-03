// Fill out your copyright notice in the Description page of Project Settings.


#include "WallLever.h"
#include "Components/SceneComponent.h"
#include "Selector.h"

// Sets default values for this component's properties
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
		grabbingSelector = cursor;
		if (cursor)
		{
			FVector worldPos = cursor->Cursor().GetLocation();
			FVector localPos = GetComponentTransform().InverseTransformPosition(worldPos);

			initialGrabX = localPos.X;
			if (LeverArm)
				initialRotation = LeverArm->GetRelativeRotation();

			cursor->GrabFocus(true);
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

void UWallLever::UpdateLeverRotation()
{
	if (!grabbingSelector || !LeverArm) return;

	// Get current cursor position in lever local space
	FVector worldPos = grabbingSelector->Cursor().GetLocation();
	FVector localPos = GetComponentTransform().InverseTransformPosition(worldPos);

	// Compute new pitch based on local X offset
	float deltaX = (localPos.X - initialGrabX) * Sensitivity;
	float targetPitch = FMath::Clamp(initialRotation.Pitch + deltaX, MinPitch, MaxPitch);

	FRotator newRot = initialRotation;
	newRot.Pitch = targetPitch;
	LeverArm->SetRelativeRotation(newRot);

	BroadcastLeverValue();
}

void UWallLever::BroadcastLeverValue()
{
	if (!LeverArm) return;

	float pitch = LeverArm->GetRelativeRotation().Pitch;
	float normalized = (pitch - MinPitch) / (MaxPitch - MinPitch);
	normalized = FMath::Clamp(normalized, 0.0f, 1.0f);

	UE_LOG(LogTemp, Warning, TEXT("Lever rotation normalized: %f"), normalized);
	LeverDelegate.Broadcast(normalized);
}

// Called every frame
void UWallLever::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	if (grabbingSelector)
	{
		UpdateLeverRotation();
	}

}