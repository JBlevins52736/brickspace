// Fill out your copyright notice in the Description page of Project Settings.


#include "HandSelector.h"

#include "Vodget.h"
#include "Net/UnrealNetwork.h" // Required for DOREPLIFETIME
#include "BrickSpacePawn.h"
#include "IXRTrackingSystem.h"
#include "GameFramework/Pawn.h"
#include "OculusXRInputFunctionLibrary.h"
#include "OculusXRHandComponent.h"
#include "MotionControllerComponent.h"
#include "WorldGrabber.h"
#define GRAB_THRESHOLD 120 // this is the squared length of the threshold to determine if you are grabbing.
#define GRAB_FINGER_COUNT 5 // this is the amount of fingers that must meet the threshold parameter to be considered a grab.
#define FINGERTIP_INDICIE_INC 5

UHandSelector::UHandSelector() : handMaterial(nullptr)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

UVodget* UHandSelector::DoRaycast()
{
	UVodget* retval = nullptr;

	// You can use this to customize various properties about the trace
	FVector StartPos = FVector::Zero();
	FVector EndPos = FVector::Zero();
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());
	if (handTrackingActive)
	{
		CalculateEyeHandPosBoneData(StartPos, EndPos); // This is necessary, the default eye hand ray places the location at your wrist, we need the point in your hand. RS
	}
	else {

		FVector EyePos = centerEye->GetComponentLocation();
		FVector HandPos = hand->GetComponentLocation();

		FVector ray = HandPos - EyePos;
		ray.Normalize();
		ray *= 20.0;

		StartPos = HandPos - ray;
		EndPos = HandPos + ray;
	}


	if (GetWorld()) { // ECC_WorldDynamic ECC_PhysicsBody ECC_Visibility
		bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, StartPos, EndPos, ECC_PhysicsBody, Params, FCollisionResponseParams());

		if (bHit)
		{
			//retval = Hit.Component->GetOwner()->GetComponentByClass<UVodget>();
			for (int i = 0; i < Hit.Component->GetNumChildrenComponents(); i++) {
				retval = Cast<UVodget>(Hit.Component->GetChildComponent(i));
				if (retval != nullptr && ((retval->selectionFilter & selectionFilter) > 0))
					break;
			}
			//UE_LOG(LogTemp, Warning, TEXT("Hit Somethin:%s"), *FString(Hit.Component->GetName()));
			//UE_LOG(LogTemp, Warning, TEXT("Hit Somethin:%s"), *FString(Hit.GetActor()->GetActorLabel()));
		}
		//DrawDebugLine(GetWorld(), StartPos, EndPos, FColor::Red, false, -1.f, 0.f, 1.f);
	}

	return retval;
}

void UHandSelector::CalculateHandSize()
{
	FVector palmPos = skRef->GetBoneLocation(palmName, EBoneSpaces::ComponentSpace);
	FVector middleIdx = skRef->GetBoneLocation(boneNames[2], EBoneSpaces::ComponentSpace);
	FVector directionToMiddleIdx = middleIdx - palmPos;
	relativeHandSizeSquared = FVector::DotProduct(directionToMiddleIdx, directionToMiddleIdx);
	rayCastPosition = (palmPos + middleIdx) * 0.5f;

}

void UHandSelector::CheckHandGestures()
{

	// Start performing bone lookups by name because Meta doesnt know what a fixed size array is apparently
	FVector palmVector = skRef->GetBoneLocation(palmName, EBoneSpaces::ComponentSpace);
	HandGrabGesture(palmVector);
	PinchGesture(palmVector);
	FlickGesture(palmVector);
}

void UHandSelector::HandGrabGesture(const FVector& palmPos)
{

	int correctOrientation = 0; // this indicates correct position for hand grab
	FVector currentPos = FVector::Zero();
	FVector directionPalmToFinger = FVector::Zero();
	float squaredLengthAvg = 0;
	float squaredLengthTotalFingers = 0;
	float relativeGrabThreshold = relativeHandSizeSquared * 0.5f;
	for (int i = 0; i < boneNames.Num(); i++) {

		currentPos = skRef->GetBoneLocation(boneNames[i], EBoneSpaces::ComponentSpace);
		directionPalmToFinger = currentPos - palmPos;
		squaredLengthTotalFingers += FVector::DotProduct(directionPalmToFinger, directionPalmToFinger);

	}
	squaredLengthAvg = squaredLengthTotalFingers / GRAB_FINGER_COUNT;
	if (squaredLengthAvg < relativeGrabThreshold && focusVodget) focusVodget->ForePinch(this, true);
	else if (focusVodget && focus_grabbed && squaredLengthAvg > relativeGrabThreshold) focusVodget->ForePinch(this, false);



}

void UHandSelector::PinchGesture(const FVector& palmPos)
{
	FVector thumbPos = skRef->GetBoneLocation(boneNames[0], EBoneSpaces::ComponentSpace);
	FVector indexPos = skRef->GetBoneLocation(boneNames[1], EBoneSpaces::ComponentSpace);
	FVector directionThumbToIndex = indexPos - thumbPos;
	float distanceSqauredThumbToIndex = FVector::DotProduct(directionThumbToIndex, directionThumbToIndex);
	float remainingFingerSquaredLenght = 0;
	float relativeGrabThreshold = relativeHandSizeSquared * 0.5f;

	for (int i = 2; i < boneNames.Num(); i++) {
		FVector fingertipPos = skRef->GetBoneLocation(boneNames[i], EBoneSpaces::ComponentSpace);
		FVector directionPalmToFinger = fingertipPos - palmPos;
		remainingFingerSquaredLenght += FVector::DotProduct(directionPalmToFinger, directionPalmToFinger);
	}
	float reaminingFingerAvgDist = remainingFingerSquaredLenght / 3;
	if (reaminingFingerAvgDist < relativeGrabThreshold && focusVodget && distanceSqauredThumbToIndex < 0.5f) focusVodget->ForePinch(this, true);
	else if (reaminingFingerAvgDist >= relativeGrabThreshold && focus_grabbed && focusVodget && distanceSqauredThumbToIndex >= 0.5f) focusVodget->ForePinch(this, false);

}

void UHandSelector::FlickGesture(const FVector& palmPos)
{
	FVector thumbPos = skRef->GetBoneLocation(boneNames[0], EBoneSpaces::ComponentSpace);
	FVector indexPos = skRef->GetBoneLocation(boneNames[1], EBoneSpaces::ComponentSpace);
	FVector directionThumbToIdx = indexPos - thumbPos;
	// Get direction wrist to thumb.
	// use thumbToIdx look for negative result
	// check the squaredLength against the wrist distance. Ensure idx finger is moved enough to trigger all events.
	directionThumbToIdx.Normalize();
	float projectionPointAlongVector = FVector::DotProduct(directionThumbToIdx, thumbPos);
	//GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Blue, FString::Printf(TEXT("projectedDistance: %f"), projectionPointAlongVector));
}

void UHandSelector::WorldGrabGesture(const FVector& palmPos)
{
	float squaredLengthOfFingers = 0;
	FVector thumbTipPos = skRef->GetBoneLocation(boneNames[0], EBoneSpaces::WorldSpace);
	FVector palmPosWorld = skRef->GetComponentTransform().TransformPosition(palmPos);
	FVector dirOfThumb = thumbTipPos - palmPosWorld;
	float lengthThumbSquared = FVector::DotProduct(dirOfThumb, dirOfThumb);
	dirOfThumb.Normalize();
	float dotOfThumbToWorldUp = FVector::DotProduct(dirOfThumb, FVector::UpVector); // I need to get the dir vector in world space and make this comp.
	//GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Blue, FString::Printf(TEXT("up dot: %f"), dotOfThumbToWorldUp));
	UWorldGrabber* worldGrabber = Cast<UWorldGrabber>(GetOwner()->GetRootComponent());
	if (dotOfThumbToWorldUp < 0.5f)
	{
		if (!isUsingWorldGrabber) return;
		

		UMotionControllerComponent* motionCont = Cast<UMotionControllerComponent>(hand->GetAttachParent());
		if (motionCont->GetTrackingSource() == EControllerHand::Right)
		{
			worldGrabber->RWorldGrab(false);
			isUsingWorldGrabber = false;
		}
		else {
			worldGrabber->LWorldGrab(false);
			isUsingWorldGrabber = false;
		}
		return;
	}
	
	worldGrabber->ActivateToggle(true);
	for (int i = 1; i < boneNames.Num(); i++)
	{
		FVector fingerPos = skRef->GetBoneLocation(boneNames[i], EBoneSpaces::ComponentSpace);
		FVector direction = fingerPos - palmPos;
		float squaredLength = FVector::DotProduct(direction, direction);
		squaredLengthOfFingers += squaredLength;
	}
	float relativeGrabThreshold = relativeHandSizeSquared * 0.5f;
	float squaredLengthAvg = squaredLengthOfFingers / 4;// number of fingers to average
	//GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Red, FString::Printf(TEXT("SquaredLengthAvg: %f, Relative comp: %f "), squaredLengthAvg, relativeGrabThreshold));
	if (!isUsingWorldGrabber && squaredLengthAvg < relativeGrabThreshold) // Activate world grabber
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Orange, TEXT("Entered world grab activate"));
		UMotionControllerComponent* motionCont = Cast<UMotionControllerComponent>(hand->GetAttachParent());
		if (motionCont->GetTrackingSource() == EControllerHand::Right)
		{
			worldGrabber->RWorldGrab(true);
			isUsingWorldGrabber = true;
		}
		else {
			worldGrabber->LWorldGrab(true);
			isUsingWorldGrabber = true;
		}
	}
	else if (isUsingWorldGrabber && squaredLengthAvg >= relativeGrabThreshold) // Deactivate world grabber
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Yellow, TEXT("Entered world grab deactivate"));
		

		UMotionControllerComponent* motionCont = Cast<UMotionControllerComponent>(hand->GetAttachParent());
		if (motionCont->GetTrackingSource() == EControllerHand::Right)
		{
			worldGrabber->RWorldGrab(false);
			isUsingWorldGrabber = false;
		}
		else {
			worldGrabber->LWorldGrab(false);
			isUsingWorldGrabber = false;
		}
	}
}

void UHandSelector::CalculateEyeHandPosBoneData(FVector& startVector, FVector& endPos)
{
	FVector palmPos = skRef->GetBoneLocation(palmName, EBoneSpaces::WorldSpace);
	FVector middleIdx = skRef->GetBoneLocation(boneNames[2], EBoneSpaces::WorldSpace);
	FVector midPointVec = (middleIdx + palmPos) * 0.5f;
	FVector eyePos = centerEye->GetComponentLocation();
	FVector ray = midPointVec - eyePos;
	ray.Normalize();
	ray *= 20.0;

	startVector = midPointVec - ray;
	endPos = midPointVec + ray;
}

inline FVector UHandSelector::GetHandMidpointPos()
{
	FVector palmPos = skRef->GetBoneLocation(palmName, EBoneSpaces::WorldSpace);
	FVector middleIdx = skRef->GetBoneLocation(boneNames[2], EBoneSpaces::WorldSpace);
	return (palmPos + middleIdx) * 0.5f;
}

// Called when the game starts
void UHandSelector::SetCursor()
{
	if (handTrackingActive) // Not placing it at this location makes the app very difficult to play with hand tracking
	{
		FTransform transform = FTransform::Identity;
		FVector midPoint = GetHandMidpointPos();
		transform.SetLocation(midPoint);
		transform.SetRotation(skRef->GetBoneQuaternion(palmName, EBoneSpaces::WorldSpace));
		transform.SetScale3D(FVector::OneVector);
		cursor = transform;
	}
	else
		cursor = hand->GetComponentTransform();

	// Search for Vodget components when focus is not grabbed.
	//if (focusVodget != nullptr) {
	//	if (focus_grabbed) {
	//		// Set cursor to the scaled world grab location when focusVodget exists and focus is grabbed.
	//		//FVector EyePos = head->GetComponentTransform().TransformPosition(dominantEyeOffset);
	//		FVector EyePos = centerEye->GetComponentLocation();
	//		FVector HandPos = hand->GetComponentLocation();

	//		// The current eye-hand dir.
	//		FVector eyeHandDir = (HandPos - EyePos);

	//		// Note: eyeHandDir is scaled by multiplying by ratio of initial hit-Eye over hand-eye distances
	//		FVector eyeHitVec = eyeHandDir * ratioHitEyeOverHandEye;

	//		cursor.SetLocation(EyePos + eyeHitVec);
	//	}
	//	else
	//	{
	//		// Set cursor to hit point when focusVodget exists.
	//		//FVector worldHitVec = cursor.TransformPosition(localHitVec);
	//		cursor.SetLocation(Hit.ImpactPoint);
	//	}
	//}
}

void UHandSelector::GrabFocus(bool val)
{
	Super::GrabFocus(val);

	if (focus_grabbed) {
		// Calculate the hit-eye / hand-eye ratio the moment focus is grabbed.
		//FVector EyePos = head->GetComponentTransform().TransformPosition(dominantEyeOffset);
		FVector EyePos = centerEye->GetComponentLocation();
		FVector HandPos = hand->GetComponentLocation();

		float eyeHitDistance = (Hit.ImpactPoint - EyePos).Length();;
		float eyeHandDistance = (HandPos - EyePos).Length();
		ratioHitEyeOverHandEye = eyeHitDistance / eyeHandDistance;
	}
}

// Called every frame
void UHandSelector::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Turn tick off in the remote ghosts.
	APawn* pawn = Cast<APawn>(GetOwner());
	if (pawn) {
		if (!pawn->IsLocallyControlled()) {
			return;
		}
	}
	handTrackingActive = UOculusXRInputFunctionLibrary::IsHandTrackingEnabled();
	if (relativeHandSizeSquared <= 0 && handTrackingActive) CalculateHandSize();
	ABrickSpacePawn* bspawn = Cast<ABrickSpacePawn>(GetOwner());
	if (bspawn && handMesh) {

		//VARLog(TEXT("UHandSelector::TickComponent"));

		if (pawn->GetLocalRole() == ROLE_Authority)
		{
			handMesh->SetWorldLocation(hand->GetComponentLocation());

		}
		else if (pawn->GetLocalRole() == ROLE_AutonomousProxy)
		{
			Server_MeshPosUpdate(this, hand->GetComponentLocation());
		}
	}

	if (!focus_grabbed)
	{


		// Use a physics raycast to find vodgets in the scene.
		UVodget* hitVodget = DoRaycast();

		// If a vodgets is found, give it focus and begin forwarding any events while it still has focus.
		if (hitVodget != focusVodget)
		{
			// Let the previous focusVodgetCurr know that it has lost focus here.
			if (focusVodget != nullptr) {
				//UE_LOG(LogTemp, Warning, TEXT("Focus FALSE:%s"), *FString(focusVodget->GetOwner()->GetActorLabel()));
				focusVodget->Focus(this, false);
			}

			focusVodget = hitVodget;

			// Let the focusVodgetCurr know that it now has selector focus here.
			if (focusVodget != nullptr) {
				//UE_LOG(LogTemp, Warning, TEXT("Focus TRUE:%s"), *FString(focusVodget->GetOwner()->GetActorLabel()));
				focusVodget->Focus(this, true);

			}


		}

	}
	if (handTrackingActive && focusVodget) CheckHandGestures();
	if (handTrackingActive && bspawn)
	{
		FVector palmPos = skRef->GetBoneLocation(palmName, EBoneSpaces::ComponentSpace);
		WorldGrabGesture(palmPos);
	}
}

void UHandSelector::SetFilter(uint16 filter)
{
	// ToDo: 
	// As players join their Hat and HandSelectors will be accessed through their Pawns and assigned 
	// roles initially and changed periodically. 
	// The hand will be changed by calling this SetFilter(...) method.

	Super::SetFilter(filter);
	//SetHandColor();
}

void UHandSelector::BeginPlay()
{
	Super::BeginPlay();
#if WITH_EDITOR
	if (skRef)
	{
		boneNames.Add(FName("Thumb Tip"));
		boneNames.Add(FName("Index Tip"));
		boneNames.Add(FName("Middle Tip"));
		boneNames.Add(FName("Ring Tip"));
		boneNames.Add(FName("Pinky Tip"));
		palmName = FName("Wrist Root");
	}
#else 
	if (skRef) {
		boneNames.Add(FName("Thumb_Tip"));
		boneNames.Add(FName("Index_Tip"));
		boneNames.Add(FName("Middle_Tip"));
		boneNames.Add(FName("Ring_Tip"));
		boneNames.Add(FName("Pinky_Tip"));
		palmName = FName("Wrist_Root");
	}

#endif
	CalculateHandSize();
}

#pragma region HAND_MESH_POSITION_REPLICATION

//void UHandSelector::OnRep_MeshPosUpdate()
//{
//	//APawn* pawn = Cast<APawn>(GetOwner());
//	//if (!pawn) {
//	//	UE_LOG(LogTemp, Error, TEXT("OnRep_MeshPosUpdate() could not cast owner to pawn in HandSelector.cpp"));
//	//	return;
//	//}
//
//	//if (handMesh && 
//	//	pawn->GetLocalRole() == ROLE_SimulatedProxy || 
//	//	(pawn->GetLocalRole() == ROLE_Authority && !pawn->IsLocallyControlled()) )
//	if ( handMesh )
//	{
//		//VARLog(TEXT("UHandSelector::OnRep_MeshPosUpdate"));
//
//		handMesh->SetWorldLocation(handPos);
//	//	else UE_LOG(LogTemp, Error, TEXT("Hand mesh is nullptr in MeshPosUpdate. HandSelector.cpp"));	
//	}
//}
#pragma endregion HAND_MESH_POSITION_REPLICATION

void UHandSelector::Server_MeshPosUpdate_Implementation(UHandSelector* handSelector, FVector handPos)
{
	//VARLog(TEXT("UHandSelector::Server_MeshPosUpdate_Implementation"));

	//if (handSelector) {
		//UHandSelector* handSelector = Cast<UHandSelector>(selector);

	handSelector->handMesh->SetWorldLocation(handPos);

	//handSelector->handPos = pos;
	//handSelector->OnRep_MeshPosUpdate();
//}
}

#pragma region HAND_MATERIAL_CHANGE_REPLICATION

void UHandSelector::SetMaterial(UMaterialInterface* color)
{
	VARLog(TEXT("USelector::SetMaterial"));

	APawn* pawn = Cast<APawn>(GetOwner());
	if (!pawn) {
		UE_LOG(LogTemp, Error, TEXT("Log() could not cast owner to pawn in HandSelector.cpp"));
		return;
	}

	if (pawn->HasAuthority()) {
		UE_LOG(LogTemp, Warning, TEXT("SetMaterial called from server client"));
		Server_SetMaterial_Implementation(color);
	}
	else if (pawn->GetLocalRole() == ROLE_AutonomousProxy) {
		UE_LOG(LogTemp, Warning, TEXT("SetMaterial called from remote client"));
		Server_SetMaterial(color);
	}
}

void UHandSelector::Server_SetMaterial_Implementation(UMaterialInterface* color)
{
	VARLog(TEXT("USelector::Server_SetMaterial_Implementation"));

	handMaterial = color;
	OnRep_Material();
}

void UHandSelector::OnRep_Material()
{
	VARLog(TEXT("USelector::OnRep_Material"));

	if (handMaterial)
		handMesh->SetMaterial(0, handMaterial);

}

void UHandSelector::VARLog(FString methodName)
{
	APawn* pawn = Cast<APawn>(GetOwner());
	if (!pawn) {
		UE_LOG(LogTemp, Error, TEXT("Log() could not cast owner to pawn in HandSelector.cpp"));
		return;
	}

	FString locstr = (pawn->IsLocallyControlled()) ? TEXT("LocallyControlled") : TEXT("NotLocallyControlled");
	switch (pawn->GetLocalRole())
	{
	case ROLE_Authority:		// Server Actor version: This is also the hosting listen client version.
		UE_LOG(LogTemp, Warning, TEXT("%s: ROLE_Authority: %s"), *methodName, *locstr);
		break;
	case ROLE_AutonomousProxy:	// Non-Server Player Actor version: IsLocallyControlled should be true
		UE_LOG(LogTemp, Warning, TEXT("%s: ROLE_AutonomousProxy: %s"), *methodName, *locstr);
		break;
	case ROLE_SimulatedProxy:	// Non-Server Ghost
		UE_LOG(LogTemp, Warning, TEXT("%s: ROLE_SimulatedProxy: %s"), *methodName, *locstr);
		break;
	}
}


#pragma endregion HAND_MATERIAL_CHANGE_REPLICATION


void UHandSelector::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHandSelector, handMaterial);
	//DOREPLIFETIME(UHandSelector, handPos);
}