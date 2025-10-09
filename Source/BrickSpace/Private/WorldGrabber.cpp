// Fill out your copyright notice in the Description page of Project Settings.
#include "WorldGrabber.h"
#include "Components/SceneComponent.h"
#include "HandSelector.h"
#include "BrickSpacePawn.h"
#include "BrickSpaceGameState.h"

// Sets default values for this component's properties
UWorldGrabber::UWorldGrabber() :
	leftHand(nullptr), rightHand(nullptr)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	// ...
	childsrt.SetIdentity();
	prevBimanualHandDist = 0.0;
}

void UWorldGrabber::SetLocalCursor()
{

	// Normal world grabbing until pos anchor is set.
	// Once pos anchor is set we enter into 2 point calibration mode.
	if (leftGrabbing && rightGrabbing)
	{

		FVector left = leftHand->GetComponentLocation();
		FVector right = rightHand->GetComponentLocation();

		if (scaleMode)
		{

			FVector saveLeft = left;

			ChangeWorldScaling(left, right);

			float ds = (saveLeft - left).Length();
			UE_LOG(LogTemp, Warning, TEXT("WorldGrabber ds:%f"), ds);

		}
		else
		{
			cursorsrt.SetScale3D(FVector::OneVector);
		}

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
	if (!activeMode)
	{
		// If active mode is not enabled then do nothing.
		return;
	}

	leftGrabbing = Value;
	//UE_LOG(LogTemp, Warning, TEXT("WorldGrabber LEFT:%s"), *FString((Value) ? "GRAB" : "RELEASE"));
	GrabChanged();
}

void UWorldGrabber::RWorldGrab(const bool Value)
{
	if (!activeMode)
	{
		// If active mode is not enabled then do nothing.
		return;
	}

	rightGrabbing = Value;
	//UE_LOG(LogTemp, Warning, TEXT("WorldGrabber RIGHT:%s"), *FString((Value) ? "GRAB" : "RELEASE"));
	GrabChanged();
}


void UWorldGrabber::GrabChanged()
{
	if (leftHand == nullptr || rightHand == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("WorldGrabber::GrabChanged: leftHand or rightHand not assigned."));
		return;
	}
	
	// This is the very moment that bimanual grabbing begins.
	if (leftGrabbing && rightGrabbing)
	{
		// Save the distance between hands when bimanual scaling is enabled. 
		StartWorldScaling(leftHand->GetComponentLocation(), rightHand->GetComponentLocation());
	}

	SetLocalCursor();

	childsrt = cursorsrt.Inverse();
}

void UWorldGrabber::DollyToggle(const bool Value) { dollyMode = !dollyMode; }
void UWorldGrabber::ScaleToggle(const bool Value) { scaleMode = !scaleMode; }
void UWorldGrabber::ActivateToggle(const bool Value)
{ 
	//activeMode = !activeMode;
	//ActiveChanged();
}


void UWorldGrabber::ActiveChanged()
{
	PrimaryComponentTick.SetTickFunctionEnable(activeMode);
}

void UWorldGrabber::UpdateCursors()
{
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

// Called every frame
void UWorldGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (leftHand == nullptr || rightHand == nullptr || !(leftGrabbing || rightGrabbing))
		return;

	UpdateCursors();
}

void UWorldGrabber::Server_Move_Implementation(UWorldGrabber* WorldGrabber, FTransform transform)
{
	WorldGrabber->SetRelativeTransform(transform);
}

void UWorldGrabber::StartWorldScaling(FVector lhand, FVector rhand)
{
	prevBimanualHandDist = (lhand - rhand).Length();
	currWorldToMeters = GetWorld()->GetWorldSettings()->WorldToMeters;
}

void UWorldGrabber::ChangeWorldScaling(FVector &lhand, FVector &rhand)
{
	// The VR Pawn is made larger/smaller in Unreal by changing the WorldToMeters setting.
	// Pawn geometry like the markers attached to controllers is the responsibility of the application.
	// https://forums.unrealengine.com/t/changing-the-player-pawn-camera-size/384747/3

	float currBimanualHandDist = (lhand - rhand).Length();
	float ds = prevBimanualHandDist / currBimanualHandDist;
UE_LOG(LogTemp, Warning, TEXT("WorldGrabber ds:%f"), ds);

	float worldScale = currWorldToMeters * ds;

	SetWorldToMeters(worldScale);

	// When worldToMeters changes all world space positional values need to be adjusted.
	FVector scaledpos = GetComponentLocation() * ds;
	SetWorldLocation(scaledpos);
	lhand *= ds;
	rhand *= ds;
	prevBimanualHandDist = (lhand - rhand).Length();
}

void UWorldGrabber::SetWorldToMeters(float worldScale)
{
	if (currWorldToMeters == worldScale)
		return;

	currWorldToMeters = worldScale;
	GetWorld()->GetWorldSettings()->WorldToMeters = currWorldToMeters;

	//UE_LOG(LogTemp, Warning, TEXT("WorldGrabber currWorldToMeters:%f"), currWorldToMeters);

	// Scale local hands immediately as local hands are not replicated
	float handScale = currWorldToMeters / 100.0;
	FVector scaleVec = FVector::OneVector * handScale;
	if (leftHand)
		leftHand->SetWorldScale3D(scaleVec);
	if (rightHand)
		rightHand->SetWorldScale3D(scaleVec);
}

