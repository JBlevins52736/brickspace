// Fill out your copyright notice in the Description page of Project Settings.


#include "Calib2Hands.h"
#include <Kismet/GameplayStatics.h>

void UCalib2Hands::SetCursorSrt(FVector leftPos, FVector rightPos)
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
	if (leftGestureState && rightGestureState && !GetOwner()->HasAuthority())
		ServerCalibRequest();
}

void UCalib2Hands::RCalibGesture(const bool Value)
{
	rightGestureState = Value;
	if (leftGestureState && rightGestureState && !GetOwner()->HasAuthority())
		ServerCalibRequest();
}

void UCalib2Hands::ServerCalibRequest_Implementation()
{
	APlayerController* ListenServerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	APawn* ListenServerPawn = ListenServerController->GetPawn();

	FVector leftPos;
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
	}

	FVector rightPos;
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
	}

	UCalib2Hands* calib = Cast<UCalib2Hands>(GetOwner()->GetRootComponent());
	if (calib) {
		calib->ClientCalibrate(leftPos, rightPos);
	}
}

void UCalib2Hands::ClientCalibrate_Implementation(FVector leftPos, FVector rightPos)
{
	// If either hand is not assigned then do nothing.
	if (leftHand == nullptr || rightHand == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Calib2Hands: leftHand or rightHand not assigned."));
		return;
	}

	// Set the childsrt to the inverse of the listen server clients cursor.
	SetCursorSrt(leftPos, rightPos);

	// Calculate the identity worldsrt as a child of the cursor.
	FTransform child = csr.Inverse();


	// My left hand is where the hosts right hand is and vis-versa
	SetCursorSrt(rightHand->GetComponentLocation(), leftHand->GetComponentLocation());

	// Move the cursor to the servers hands.
	FTransform worldsrt = child * csr;
	FTransform pawnChildOfWorld = GetRelativeTransform() * worldsrt.Inverse();

	Server_Move(this, pawnChildOfWorld);
}
