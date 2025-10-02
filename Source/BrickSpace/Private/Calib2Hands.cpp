// Fill out your copyright notice in the Description page of Project Settings.


#include "Calib2Hands.h"
#include <Kismet/GameplayStatics.h>

void UCalib2Hands::SetLocalCursor()
{
	if (GetOwner()->HasAuthority()) {
		Super::SetLocalCursor();
		return;
	}
	// Set the cursor to the local hands.
	cursorsrt.SetLocation((leftPos + rightPos) * 0.5);
	FVector xAxis = leftPos - rightPos;
	xAxis.Z = 0.0;
	FQuat rot = FRotationMatrix::MakeFromXZ(xAxis, FVector::UpVector).ToQuat();
	cursorsrt.SetRotation(rot);
	cursorsrt.SetScale3D(FVector::OneVector);
}

void UCalib2Hands::ActiveChanged()
{
	if (GetOwner()->HasAuthority()) {
		// The server is always calibrated operating in standard world grabber replicated mode.
		Super::ActiveChanged();
		return;
	}

	// Clients calibrate to the servers hands immediately and don't need tick update.
	if (activeMode) {

		// If either hand is not assigned then do nothing.
		if (leftHand == nullptr || rightHand == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Calib2Hands::HandsTouchingServers: leftHand or rightHand not assigned."));
			return;
		}

		leftPos = leftHand->GetComponentLocation();
		rightPos = rightHand->GetComponentLocation();
		SetLocalCursor();

		// Calculate the identity worldsrt as a child of the cursor.
		childsrt = cursorsrt.Inverse();

		// Get the servers simulated hand locations from its pawn
		APlayerController* ListenServerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		APawn* ListenServerPawn = ListenServerController->GetPawn();
		// Get all SceneComponents with the tag "MyTag" on this actor
		UActorComponent* actorComponent = ListenServerPawn->FindComponentByTag(USceneComponent::StaticClass(), FName("LeftHand"));
		if (!actorComponent)
		{
			UE_LOG(LogTemp, Error, TEXT("No component with LeftHand tag found."));
			return;
		}
		USceneComponent* handComponent = Cast<USceneComponent>(actorComponent);
		leftPos = handComponent->GetComponentLocation();

		actorComponent = ListenServerPawn->FindComponentByTag(USceneComponent::StaticClass(), FName("RightHand"));
		if (!handComponent)
		{
			UE_LOG(LogTemp, Error, TEXT("No component with RightHand tag found."));
			return;
		}
		handComponent = Cast<USceneComponent>(actorComponent);
		rightPos = handComponent->GetComponentLocation();

		// Move the cursor to the servers hands.
		UpdateCursors();

		// World grabbing is always disabled for clients.
		activeMode = false;
	}
}
