// Fill out your copyright notice in the Description page of Project Settings.
#include "WorldGrabber.h"
#include "Components/SceneComponent.h"
#include "HandSelector.h"
#include "BrickSpacePawn.h"
#include "BrickSpaceGameState.h"
#include <Kismet/GameplayStatics.h>

// Sets default values for this component's properties
UWorldGrabber::UWorldGrabber() :
	leftHand(nullptr), rightHand(nullptr)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	childsrt.SetIdentity();
	initialBimanualHandDist = 0.0;
}

void UWorldGrabber::SetLocalCursor()
{
	if (leftGrabbing && rightGrabbing)
	{
		FVector left = leftHand->GetComponentLocation();
		FVector right = rightHand->GetComponentLocation();

		// Set the cursor position to the average world location of both hands
		cursorsrt.SetLocation((left + right) * 0.5);

		// Set the cursor rotation to the FRotationMatrix::MakeFromXZ where X is the vector between the hands and Z is up.
		FVector xAxis = left - right;

		if (dollyMode)
		{
			xAxis.Z = 0.0;
		}
		FQuat rot = FRotationMatrix::MakeFromXZ(xAxis, FVector::UpVector).ToQuat();

		cursorsrt.SetRotation(rot);

		// Only the server should change WorldToMeters property
		if (scaleMode && GetOwner()->GetLocalRole() == ROLE_Authority)
		{
			// Set currBimanualHandDist to the actual distance next.
			float currBimanualHandDist = (left - right).Length();

			// The VR Pawn is made larger/smaller in Unreal by changing the WorldToMeters setting.
			// Pawn geometry like the markers attached to controllers is the responsibility of the application.
			// https://forums.unrealengine.com/t/changing-the-player-pawn-camera-size/384747/3

			// As scale makes a VR pawn larger or smaller it changes the bimanual hand distance.
			// The players bimanual hand distance needs to be adjusted to the initial scale.
			float ds = initialWorldToMeters / currWorldToMeters;
			currBimanualHandDist *= ds;

			// The direct world scaling ratio is opposite cursor scaling.
			ds = initialBimanualHandDist / currBimanualHandDist;

			currWorldToMeters = initialWorldToMeters * ds;

			ABrickSpaceGameState* GameState = Cast<ABrickSpaceGameState>( UGameplayStatics::GetGameState(GetWorld()) );
			if (GameState)
			{
				GameState->currWorldToMeters = currWorldToMeters;
				GameState->OnRep_WorldScale();
			}
			//GetWorld()->GetWorldSettings()->WorldToMeters = currWorldToMeters;
			//OnRep_WorldScale();


			//// Scale the Pawns geometry, Note: WorldToMeters base is 100.
			//float handScale = currWorldToMeters / 100.0;
			//leftHand->SetWorldScale3D(FVector::OneVector * handScale);
			//rightHand->SetWorldScale3D(FVector::OneVector * handScale);

		}
		//else if (scaleMode) {
		//	// Scale the Pawns geometry, Note: WorldToMeters base is 100.
		//	float handScale = currWorldToMeters / 100.0;
		//	UE_LOG(LogTemp, Warning, TEXT("WorldGrabber handScale:%f"), handScale);

		//	ABrickSpacePawn* brickSpacePawn = Cast<ABrickSpacePawn>(GetOwner());
		//	brickSpacePawn->Server_MeshScaleUpdate(leftHand, rightHand, handScale);
		//}
		else
		{
			cursorsrt.SetScale3D(FVector::OneVector);
		}
	}
	else if (leftGrabbing)
	{
		cursorsrt = leftHand->GetComponentTransform();
		cursorsrt.SetRotation(FQuat::Identity);
		cursorsrt.SetScale3D(FVector::OneVector);
	}
	else if (rightGrabbing)
	{
		cursorsrt = rightHand->GetComponentTransform();
		cursorsrt.SetRotation(FQuat::Identity);
		cursorsrt.SetScale3D(FVector::OneVector);
	}
}

void UWorldGrabber::LWorldGrab(const bool Value)
{
	leftGrabbing = Value;
	UE_LOG(LogTemp, Warning, TEXT("WorldGrabber LEFT:%s"), *FString((Value) ? "GRAB" : "RELEASE"));
	GrabChanged();
}

void UWorldGrabber::RWorldGrab(const bool Value)
{
	rightGrabbing = Value;
	UE_LOG(LogTemp, Warning, TEXT("WorldGrabber RIGHT:%s"), *FString((Value) ? "GRAB" : "RELEASE"));
	GrabChanged();
}

void UWorldGrabber::GrabChanged()
{
	if (leftHand == nullptr || rightHand == nullptr)
		return;

	if (!leftGrabbing && !rightGrabbing) {
		PrimaryComponentTick.SetTickFunctionEnable(false);
		return;
	}
	if (!PrimaryComponentTick.IsTickFunctionEnabled()) {
		PrimaryComponentTick.SetTickFunctionEnable(true);
	}

	// This is the very moment that bimanual grabbing begins.
	if (leftGrabbing && rightGrabbing)
	{
		// Save the distance between hands when bimanual scaling is enabled. 
		FVector left = leftHand->GetComponentLocation();
		FVector right = rightHand->GetComponentLocation();
		initialBimanualHandDist = (left - right).Length();
		if (scaleMode) {
			initialWorldToMeters = currWorldToMeters;
		}
	}

	SetLocalCursor();

	childsrt = cursorsrt.Inverse();
}


void UWorldGrabber::DollyToggle(const bool Value) { dollyMode = !dollyMode; }
void UWorldGrabber::ScaleToggle(const bool Value) { scaleMode = !scaleMode; }

// Called every frame
void UWorldGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!scaleMode || leftHand == nullptr || rightHand == nullptr || !(leftGrabbing || rightGrabbing))
		return;

	SetLocalCursor();
	FTransform worldsrt = childsrt * cursorsrt;
	FTransform pawnChildOfWorld = GetRelativeTransform() * worldsrt.Inverse();

	ABrickSpacePawn* pawn = Cast<ABrickSpacePawn>(GetOwner());
	if (pawn->HasAuthority()) {
		SetRelativeTransform(pawnChildOfWorld);
	}
	else {
		Server_Move(this, pawnChildOfWorld);
	}
}

void UWorldGrabber::Server_Move_Implementation(UWorldGrabber* WorldGrabber, FTransform transform)
{
	WorldGrabber->SetRelativeTransform(transform);
}

void UWorldGrabber::OnRep_WorldScale(float worldScale)
{
	// The BrickSpaceGameState currWorldToMeters property was just changed by the listening server client.
	// Update our local copy of currWorldToMeters and scale the local and replicated hand meshes.
	currWorldToMeters = worldScale;
	UE_LOG(LogTemp, Warning, TEXT("WorldGrabber currWorldToMeters:%f"), currWorldToMeters);

	float handScale = currWorldToMeters / 100.0;
	FVector scaleVec = FVector::OneVector * handScale;
	UE_LOG(LogTemp, Warning, TEXT("WorldGrabber handScale:%f"), handScale);

	// Scale local hands immediately as local hands are not replicated
	if (leftHand)
		leftHand->SetWorldScale3D(scaleVec);
	if (rightHand)
		rightHand->SetWorldScale3D(scaleVec);

	// Scale replicated hands via server so all clients are updated.
	if (leftHandReplicated && rightHandReplicated)
	{
		ABrickSpacePawn* brickSpacePawn = Cast<ABrickSpacePawn>(GetOwner());
		if (brickSpacePawn->HasAuthority()) {
			// Scale replicated hands immediately when listen server client is updated. 
			leftHandReplicated->SetWorldScale3D(scaleVec);
			rightHandReplicated->SetWorldScale3D(scaleVec);
		}
		else {
			// Have server update replicated mesh scales.
			brickSpacePawn->Server_MeshScaleUpdate(leftHandReplicated, rightHandReplicated, handScale);
		}
	}
}

#ifdef BLAH_UNUSED
void UWorldGrabber::CalibrateHands()
{
	if (leftHand == nullptr || rightHand == nullptr || leftHandReplicated == nullptr || rightHandReplicated == nullptr)
		return;

	// For some reason the replicated hand components are not in the same place as sent to the server.
	// This method calculates the offset between local and replicated hands and moves the pawn to match.
	// This is only called on clients when the server changes the WorldToMeters property.
	// It assumes the local hands are correct and moves the pawn to match the replicated hands.
	// I don't know why the replicated hands don't match and suspect it might have something to do with the different
	// tracking origins between server and client headsets and would be gobsmacked if this calibrates them correctly (lol).

	FVector left = leftHand->GetComponentLocation();
	FVector right = rightHand->GetComponentLocation();
	FVector leftRepl = leftHandReplicated->GetComponentLocation();
	FVector rightRepl = rightHandReplicated->GetComponentLocation();

	// Set the cursor to the local hands.
	cursorsrt.SetLocation((left + right) * 0.5);
	FVector xAxis = left - right;
	xAxis.Z = 0.0;
	FQuat rot = FRotationMatrix::MakeFromXZ(xAxis, FVector::UpVector).ToQuat();
	cursorsrt.SetRotation(rot);
	cursorsrt.SetScale3D(FVector::OneVector);

	// Calculate the identity worldsrt as a child of the cursor.
	childsrt = cursorsrt.Inverse();

	// Move the cursor to the replicated hands.
	cursorsrt.SetLocation((leftRepl + rightRepl) * 0.5);
	xAxis = leftRepl - rightRepl;
	xAxis.Z = 0.0;
	rot = FRotationMatrix::MakeFromXZ(xAxis, FVector::UpVector).ToQuat();
	cursorsrt.SetRotation(rot);

	// Calculate the new worldsrt by combining the childsrt and cursorsrt.
	FTransform worldsrt = childsrt * cursorsrt;

	// Calculate where our pawn would be as a child of the worldsrt model.
	FTransform pawnChildOfWorld = GetRelativeTransform() * worldsrt.Inverse();

	// Move the pawn to match the replicated hands.
	this->SetRelativeTransform(pawnChildOfWorld);
}
#endif
