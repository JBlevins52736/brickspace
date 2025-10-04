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
	if (!WallToMove)
	{
		UE_LOG(LogTemp, Warning, TEXT("WallToMove not assigned!"));
		return;
	}

	WallStartLocation = WallToMove->GetRelativeLocation();
	CrankTargetZ = WallStartLocation.Z;
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
	float grabAxis = localPos.Y;
	float delta = grabAxis - initialGrabZ;
	float rotationDelta = delta * Sensitivity;
	float targetAngle = FMath::Clamp(initialRotation.Pitch + rotationDelta, MinPitch, MaxPitch);

	FRotator currentRot = LeverArm->GetRelativeRotation();
	FRotator targetRot = currentRot;
	targetRot.Roll = targetAngle; // adjust axis
	FRotator smoothedRot = FMath::RInterpTo(currentRot, targetRot, GetWorld()->GetDeltaSeconds(), 10.f);
	LeverArm->SetRelativeRotation(smoothedRot);

	// Normalized lever value
	/*float normalized = (targetAngle - MinPitch) / (MaxPitch - MinPitch);*/
	float normalized = FMath::Clamp(normalized, MinPitch, MaxPitch);
	/*LeverDelegate.Broadcast(normalized);*/
	UE_LOG(LogTemp, Log, TEXT("Rotation:%f"), normalized);
	// Crank logic
	if (!WallToMove) return;

	float threshold = MinPitch - 10.f;

	if (normalized >= threshold && CurrentCrankStep < MaxCrankSteps)
	{
		bCrankingForward = true;
		bCrankingBackward = false;
	}
	else if (normalized <= (MaxPitch - 10.f) && CurrentCrankStep > 0)
	{
		bCrankingForward = false;
		bCrankingBackward = true;
	}

	FVector Wall = WallToMove->GetRelativeLocation();
	UE_LOG(LogTemp, Log, TEXT("Cube Z value:%f"),Wall.Z );
}
// Called every frame
void UWallLever::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!WallToMove) {
		
			UE_LOG(LogTemp, Log, TEXT("Wall Not Found"));
			return;
	}

	// Update lever rotation if being grabbed
	if (grabbingSelector)
	{
		UpdateLeverFromSelector(grabbingSelector);
	}

	FVector loc = WallToMove->GetRelativeLocation();

	// Crank down
	if (bCrankingForward && CurrentCrankStep < MaxCrankSteps)
	{
		CrankTargetZ -= CrankUnit;
		CurrentCrankStep++;
		bCrankingForward = false; // wait for next lever trigger
	}

	// Crank up
	if (bCrankingBackward && CurrentCrankStep > 0)
	{
		CrankTargetZ += CrankUnit;
		CurrentCrankStep--;
		bCrankingBackward = false; // wait for next lever trigger
	}

	// Smoothly move wall toward target
	loc.Z = FMath::FInterpTo(loc.Z, CrankTargetZ, DeltaTime, CrankSpeed);
	WallToMove->SetRelativeLocation(loc);
	UE_LOG(LogTemp, Log, TEXT("Wall Z: %f, Target Z: %f, CurrentStep: %d"), loc.Z, CrankTargetZ, CurrentCrankStep);
}