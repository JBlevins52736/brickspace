// Fill out your copyright notice in the Description page of Project Settings.
#include "WorldGrabber.h"
#include "Components/SceneComponent.h"
#include "Net/UnrealNetwork.h" // Required for DOREPLIFETIME
#include "HandSelector.h"
#include "Runtime/MovieSceneTracks/Private/MovieSceneTracksCustomAccessors.h"

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

//
//// Called when the game starts
//void UWorldGrabber::BeginPlay()
//{
//	Super::BeginPlay();
//
//	APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
//	if (playerController != nullptr)
//	{
//		GetOwner()->EnableInput(playerController);
//
//		UInputComponent* inputComponent = playerController->InputComponent;
//		if (UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(inputComponent))
//		{
//			const UInputAction* action;
//
//			action = AVRPawn::_instance->GetAction("IA_LWorldGrab");
//			//if (action != nullptr)
//			//	Input->BindAction(action, ETriggerEvent::Triggered, this, &UWorldGrabber::LWorldGrab);
//
//			action = AVRPawn::_instance->GetAction("IA_RWorldGrab");
//			if (action != nullptr)
//				Input->BindAction(action, ETriggerEvent::Triggered, this, &UWorldGrabber::RWorldGrab);
//
//			action = AVRPawn::_instance->GetAction("IA_DollyToggle");
//			if (action != nullptr)
//				Input->BindAction(action, ETriggerEvent::Triggered, this, &UWorldGrabber::DollyToggle);
//
//			action = AVRPawn::_instance->GetAction("IA_ScaleToggle");
//			if (action != nullptr)
//				Input->BindAction(action, ETriggerEvent::Triggered, this, &UWorldGrabber::ScaleToggle);
//		}
//	}
//}

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

		//// If dollyMode constrain rotation to rotate about Z up only.
		//if (dollyMode)
		//{
		//	// Constrain cursor rotation to maintain Z up vector.		
		//	FVector wonkyup = rot.RotateVector(FVector::UpVector);
		//	FQuat dq = FQuat::FindBetweenVectors(wonkyup, FVector::UpVector);
		//	rot = dq * rot;
		//	//rot = rot * dq;
		//}
		cursorsrt.SetRotation(rot);

		if (scaleMode) {

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
			GetWorld()->GetWorldSettings()->WorldToMeters = currWorldToMeters;
			//OnRep_WorldScale();

			// Scale the Pawns geometry, Note: WorldToMeters base is 100.
			float handScale = currWorldToMeters / 100.0;
			leftHand->SetWorldScale3D(FVector::OneVector * handScale);
			rightHand->SetWorldScale3D(FVector::OneVector * handScale);

		}
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
	//UE_LOG(LogTemp, Warning, TEXT("WorldGrabber LEFT:%s"), *FString((lController != nullptr) ? "GRAB" : "RELEASE"));
	GrabChanged();
}

void UWorldGrabber::RWorldGrab(const bool Value)
{
	rightGrabbing = Value;
	//UE_LOG(LogTemp, Warning, TEXT("WorldGrabber RIGHT:%s"), *FString((rController != nullptr) ? "GRAB" : "RELEASE"));
	GrabChanged();
}

void UWorldGrabber::GrabChanged()
{
	if (leftHand == nullptr || rightHand == nullptr)
		return;

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

	// Turn tick off in the remote ghosts.
	APawn* pawn = Cast<APawn>(GetOwner());
	if (pawn) {
		if (!pawn->IsLocallyControlled())
			PrimaryComponentTick.SetTickFunctionEnable(false);
	}


	if (leftHand == nullptr || rightHand == nullptr || !(leftGrabbing || rightGrabbing) )
		return;

	SetLocalCursor();
	FTransform worldsrt = childsrt * cursorsrt;
	FTransform pawnChildOfWorld = GetRelativeTransform() * worldsrt.Inverse();

	if ( scaleMode )
		this->SetRelativeTransform(pawnChildOfWorld);
}

//void UWorldGrabber::OnRep_WorldScale()
//{
//	GetWorld()->GetWorldSettings()->WorldToMeters = currWorldToMeters;
//}
//
//void UWorldGrabber::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//	DOREPLIFETIME(UWorldGrabber, currWorldToMeters);
//}