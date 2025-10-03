// Fill out your copyright notice in the Description page of Project Settings.


#include "WallLever.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Selector.h"

// Sets default values for this component's properties
UWallLever::UWallLever()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UWallLever::BeginPlay()
{
	Super::BeginPlay();

	if (LeverArm)
	{
		InitialRotation = LeverArm->GetRelativeRotation();
	}
	
}

void UWallLever::Focus(USelector* cursor, bool state)
{
	Super::Focus(cursor, state);

	// Optional: highlight lever when focused
}

void UWallLever::ForePinch(USelector* cursor, bool state)
{
	Super::ForePinch(cursor, state); // call UGrabber's behavior first

	if (state && grabbingSelector)
	{
		// Lever-specific grab setup
		FVector WorldPos = cursor->Cursor().GetLocation();
		FVector LocalPos = GetComponentTransform().InverseTransformPosition(WorldPos);
		InitialGrabX = LocalPos.X;
		InitialRotation = LeverArm->GetRelativeRotation();
	}
}

void UWallLever::UpdateLeverRotation()
{
	if (!grabbingSelector) return;

	FVector WorldPos = grabbingSelector->Cursor().GetLocation();
	FVector LocalPos = GetComponentTransform().InverseTransformPosition(WorldPos);

	float CurrentX = LocalPos.X;
	float DeltaX = CurrentX - InitialGrabX;

	float RotationDelta = DeltaX * Sensitivity;
	float TargetPitch = FMath::Clamp(InitialRotation.Pitch + RotationDelta, MinPitch, MaxPitch);

	FRotator NewRotation = InitialRotation;
	NewRotation.Pitch = TargetPitch;

	LeverArm->SetRelativeRotation(NewRotation);
	

	BroadcastLeverValue();
}

void UWallLever::BroadcastLeverValue()
{
	float CurrentPitch = LeverArm->GetRelativeRotation().Pitch;
	float NormalizedValue = (CurrentPitch - MinPitch) / (MaxPitch - MinPitch);
	NormalizedValue = FMath::Clamp(NormalizedValue, 0.0f, 1.0f);
	UE_LOG(LogTemp, Warning, TEXT("Rotation: %f"), NormalizedValue);
	LeverDelegate.Broadcast(NormalizedValue);
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

