// Fill out your copyright notice in the Description page of Project Settings.


#include "Calib2Hands.h"
#include <Kismet/GameplayStatics.h>

void UCalib2Hands::SetCursorSrt()
{
	// Set the cursor to the local hands.
	csr.SetLocation((leftPos + rightPos) * 0.5);
	FVector xAxis = leftPos - rightPos;
	xAxis.Z = 0.0;
	FQuat rot = FRotationMatrix::MakeFromXZ(xAxis, FVector::UpVector).ToQuat();
	csr.SetRotation(rot);
	csr.SetScale3D(FVector::OneVector);
}

void UCalib2Hands::LCalibGesture(const bool Value)
{
	leftGestureState = Value;
	if (leftGestureState && rightGestureState && !GetOwner()->HasAuthority() )
		DoCalibration();
}

void UCalib2Hands::RCalibGesture(const bool Value)
{
	rightGestureState = Value;
	if (leftGestureState && rightGestureState && !GetOwner()->HasAuthority())
		DoCalibration();
}

void UCalib2Hands::DoCalibration()
{
	// If either hand is not assigned then do nothing.
	if (leftHand == nullptr || rightHand == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Calib2Hands: leftHand or rightHand not assigned."));
		return;
	}

	// Set the childsrt to the inverse of the listen server clients cursor.
	FTransform child;

	{
		// Get the servers simulated hand locations from its pawn
		APlayerController* ListenServerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (!ListenServerController)
			return;

		APawn* ListenServerPawn = ListenServerController->GetPawn();
		// Get all SceneComponents with the tag "MyTag" on this actor
		{
			UActorComponent* actorComponent = ListenServerPawn->FindComponentByTag(USceneComponent::StaticClass(), FName("LeftHand"));
			if (!actorComponent)
			{
				UE_LOG(LogTemp, Error, TEXT("Calib2Hands: No component with LeftHand tag found on ListenServerPawn."));
				return;
			}
			USceneComponent* handComponent = Cast<USceneComponent>(actorComponent);
			if (!handComponent)
			{
				UE_LOG(LogTemp, Error, TEXT("Calib2Hands: ActorComponent not a SceneComponent."));
				return;
			}
			leftPos = handComponent->GetComponentLocation();

			if (dummyL) {
				dummyL->SetWorldLocation(leftPos);
			}
		}

		{
			UActorComponent* actorComponent = ListenServerPawn->FindComponentByTag(USceneComponent::StaticClass(), FName("RightHand"));
			if (!actorComponent)
			{
				UE_LOG(LogTemp, Error, TEXT("Calib2Hands: No component with RightHand tag found on ListenServerPawn."));
				return;
			}
			USceneComponent* handComponent = Cast<USceneComponent>(actorComponent);
			if (!handComponent)
			{
				UE_LOG(LogTemp, Error, TEXT("Calib2Hands: ActorComponent not a SceneComponent."));
				return;
			}
			handComponent = Cast<USceneComponent>(actorComponent);
			rightPos = handComponent->GetComponentLocation();
			if (dummyR) {
				dummyR->SetWorldLocation(rightPos);
			}
		}

		SetCursorSrt();

		// Calculate the identity worldsrt as a child of the cursor.
		child = csr.Inverse();
	}

	// My left hand is where the hosts right hand is and vis-versa
	rightPos = leftHand->GetComponentLocation();
	leftPos = rightHand->GetComponentLocation();
	SetCursorSrt();

	// Move the cursor to the servers hands.
	FTransform worldsrt = child * csr;
	FTransform pawnChildOfWorld = GetRelativeTransform() * worldsrt.Inverse();

#ifdef BLAH
	//Server_Move(this, pawnChildOfWorld);
#endif
}
