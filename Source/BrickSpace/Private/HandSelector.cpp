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
#define GRAB_FINGER_COUNT 4 // this is the amount of fingers that must meet the threshold parameter to be considered a grab.
#define FINGERTIP_INDICIE_INC 5
#define PALM_MOTION_LIMITS 0.7f
#define CONTROLLER_MENU_ROTATION_LOW_lIMIT -0.2f
#define EYE_VIEW_VECTOR_MIN 0.6f
#define PALM_TO_EYE_VEC_MIN -0.1f
#define DISTANCE_EYE_TO_HAND_SCALAR 0.2f
#define HAND_SIZE_DISTANCE_SCALAR 0.7f;

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
	FVector eyeToPalm = palmPos - centerEye->GetComponentLocation();

	squaredHandToEyeDistance = FVector::DotProduct(eyeToPalm, eyeToPalm);

	FVector indexFinger = skRef->GetBoneLocation(boneNames[1], EBoneSpaces::ComponentSpace);
	FVector thumbPos = skRef->GetBoneLocation(boneNames[0], EBoneSpaces::ComponentSpace);
	FVector directionThumbToIndexFinger = indexFinger - thumbPos;
	relativeDistanceBetweenThumbAndIdx = FVector::DotProduct(directionThumbToIndexFinger, directionThumbToIndexFinger);

}

void UHandSelector::CheckHandGestures(float deltaTime)
{

	// Start performing bone lookups by name because Meta doesnt know what a fixed size array is apparently
	timeControlMenuButtonPresses += deltaTime;
	if (timeControlMenuButtonPresses > 0.050) {

		FVector palmVector = skRef->GetBoneLocation(palmName, EBoneSpaces::ComponentSpace);
		HandGrabGesture(palmVector);
		timeControlMenuButtonPresses = 0;
	}

}

void UHandSelector::UpdatePalmTrackingPoint(float deltaTime)
{
	if (handTrackingActive) {
		FVector currentPalmPos = skRef->GetBoneLocation(palmName, EBoneSpaces::WorldSpace);
		handTravelDirection = currentPalmPos - palmPreviousState;
		handTravelDirection.Normalize();
		float fwdDirectionCheck = FVector::DotProduct(handTravelDirection, GetOwner()->GetActorForwardVector());
		handTravelDirection = fwdDirectionCheck < -0.5f ? -handTravelDirection: handTravelDirection;
		FVector pawnPos = GetOwner()->GetActorLocation();
		FVector distanceFromPalmPrevStateToPawn = (pawnPos - currentPalmPos) + (GetOwner()->GetActorForwardVector() * 10); // move an extra 10cm in front of the desired location
		magnitudeForPredictiveHandTravel = FVector::DotProduct(distanceFromPalmPrevStateToPawn, distanceFromPalmPrevStateToPawn);
		palmPreviousState = currentPalmPos;

	}

	else {
		FVector pawnPos = GetOwner()->GetActorLocation();
		FVector distanceFromPalmPrevStateToPawn = (pawnPos - palmPreviousState) + (GetOwner()->GetActorForwardVector() * 10); // move an extra 10cm in front of the desired location
		magnitudeForPredictiveHandTravel = FVector::DotProduct(distanceFromPalmPrevStateToPawn, distanceFromPalmPrevStateToPawn);
	}
}

void UHandSelector::HandGrabGesture(const FVector& palmPos)
{


	float relativeGrabThreshold = relativeDistanceBetweenThumbAndIdx * 0.5f;
	float squaredLength = 0.0f;
	FVector thumbPos = skRef->GetBoneLocation(boneNames[0], EBoneSpaces::ComponentSpace);
	FVector indexFinger = skRef->GetBoneLocation(boneNames[1], EBoneSpaces::ComponentSpace);
	FVector direction = indexFinger - thumbPos;
	squaredLength = FVector::DotProduct(direction, direction);


	float deadZone = relativeDistanceBetweenThumbAndIdx * 0.05f;
	bool pinchChanged = false;
	if (isPinching)
	{
		if (squaredLength > relativeGrabThreshold + deadZone) {
			isPinching = false;
			pinchChanged = true;
		}
	}
	else {
		if (squaredLength < (relativeGrabThreshold + deadZone))
		{
			isPinching = true;
			pinchChanged = true;
		}
	}

	if (pinchChanged)
	{
		OnPinch.Broadcast(isPinching);

		if (focusVodget) {
			focusVodget->ForePinch(this, isPinching);

		}
	}

	//if (squaredLengthAvg < relativeGrabThreshold && focusVodget && !focus_grabbed)
	//{
	//	focusVodget->ForePinch(this, true);
	//	OnPinch.Broadcast(true);
	//}
	//else if (squaredLengthAvg > relativeGrabThreshold && focusVodget && focus_grabbed) {
	//	focusVodget->ForePinch(this, false);
	//	OnPinch.Broadcast(false);
	//}
	//



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
	if (!isUsingWorldGrabber && squaredLengthAvg < relativeGrabThreshold && lengthThumbSquared > relativeGrabThreshold) // Activate world grabber
	{
		//GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Orange, TEXT("Entered world grab activate"));
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
		//GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Yellow, TEXT("Entered world grab deactivate"));


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

	FVector midPointVec = GetMidPointBetweenThumbIndex();
	FVector eyePos = centerEye->GetComponentLocation();
	FVector ray = midPointVec - eyePos;
	ray.Normalize();
	ray *= 50.0;

	startVector = midPointVec - ray;
	endPos = midPointVec + ray;
}

inline FVector UHandSelector::GetMidPointBetweenThumbIndex()
{
	FVector indexFingerTip = skRef->GetBoneLocation(boneNames[1], EBoneSpaces::WorldSpace);
	FVector thumbTip = skRef->GetBoneLocation(boneNames[0], EBoneSpaces::WorldSpace);
	return (indexFingerTip + thumbTip) * 0.5f;
}

void UHandSelector::DetectActivationMenuSystem()
{

	if (!skRef) return;
	if (handTrackingActive) {

		FVector wristPos = skRef->GetBoneLocation(palmName, EBoneSpaces::WorldSpace);
		FVector middleTip = skRef->GetBoneLocation(boneNames[2], EBoneSpaces::WorldSpace); // middle finget tip
		FQuat wristRot = skRef->GetBoneRotationByName(palmName, EBoneSpaces::WorldSpace).Quaternion();
		FVector wristNormal = -wristRot.GetUpVector();
		FVector wristRightVec = wristRot.GetRightVector();
		FVector midpoint = (wristPos + middleTip) * 0.5f;
		FVector midpointToEye = (centerEye->GetComponentLocation() - midpoint);
		float squaredDistancePalmToEye = FVector::DotProduct(midpointToEye, midpointToEye);
		float relativeHandToEyeThreshold = squaredHandToEyeDistance * DISTANCE_EYE_TO_HAND_SCALAR;
		midpointToEye.Normalize();
		float handToEyeNorm = FVector::DotProduct(midpointToEye, wristNormal);
		float eyeToHandResult = FVector::DotProduct(centerEye->GetForwardVector(), -midpointToEye);
		float secondaryCheckWristRight = FVector::DotProduct(centerEye->GetRightVector(), wristRightVec);
		if (handToEyeNorm <= PALM_TO_EYE_VEC_MIN && eyeToHandResult > EYE_VIEW_VECTOR_MIN && FMath::Abs(secondaryCheckWristRight) < 0.2f) {
			menuSubsystemActor->SetActive(true);
			menuSubsystemActor->SetVisibility(true, true);
			FVector centerToMidpoint = midpoint - centerEye->GetComponentLocation();
			FQuat rotation = midpointToEye.Rotation().Quaternion();
			menuSubsystemActor->SetWorldRotation(rotation);
		}
		else {
			menuSubsystemActor->SetVisibility(false, true);
			menuSubsystemActor->SetActive(false);
		}
	}
	else {
		FVector controllerFwdVec = hand->GetForwardVector();
		FVector eyeFwd = centerEye->GetForwardVector();
		float opposingView = FVector::DotProduct(eyeFwd, controllerFwdVec);
		if (opposingView < CONTROLLER_MENU_ROTATION_LOW_lIMIT) {
			menuSubsystemActor->Activate();
			menuSubsystemActor->SetVisibility(true);
			FVector centerToMidpoint = centerEye->GetComponentLocation() - hand->GetComponentLocation();
			FQuat rotation = centerToMidpoint.Rotation().Quaternion();
			menuSubsystemActor->SetWorldRotation(rotation);
		}
		else {
			menuSubsystemActor->SetVisibility(false);
			menuSubsystemActor->Deactivate();
		}
	}
}

// Called when the game starts
void UHandSelector::SetCursor()
{
	bool status = Cast<UMotionControllerComponent>(hand->GetAttachParent())->IsTracked();
	if (handTrackingActive) // Not placing it at this location makes the app very difficult to play with hand tracking
	{
		FTransform transform = FTransform::Identity;
		FVector midPoint = GetMidPointBetweenThumbIndex();
		transform.SetLocation(midPoint);
		transform.SetRotation(skRef->GetBoneQuaternion(palmName, EBoneSpaces::WorldSpace));
		transform.SetScale3D(FVector::OneVector);
		cursor = transform;
	}
	else if (!handTrackingActive && focus_grabbed) // Update this section, use the component space of the hand, then use the pawn.Inverse() which should give me a rough placement
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Red, TEXT("Entered into inactive status of hand and controll"));
		FTransform transform = FTransform::Identity;
		float length = FMath::Sqrt(magnitudeForPredictiveHandTravel); // magnitudeForPredictiveHandTravel is squared length calculated in the hot path, this makes it so sqrt only happens if required.
		FVector handTravelPosition = (palmPreviousState + handTravelDirection * length); 
		
		transform.SetLocation(handTravelPosition);
		transform.SetRotation(skRef->GetBoneQuaternion(palmName, EBoneSpaces::WorldSpace));
		transform.SetScale3D(FVector::OneVector);
		
	}
	else
	{

		cursor = hand->GetComponentTransform();
	}

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

	handTrackingActive = UOculusXRInputFunctionLibrary::IsHandPositionValid(skRef->SkeletonType);

	if (relativeHandSizeSquared <= 0 && handTrackingActive) CalculateHandSize();
	ABrickSpacePawn* bspawn = Cast<ABrickSpacePawn>(GetOwner());

	if (bspawn && handMesh) {

		FVector markerPos = (handTrackingActive) ? GetMidPointBetweenThumbIndex() : hand->GetComponentLocation();

		if (pawn->GetLocalRole() == ROLE_Authority)
		{
			handMesh->SetWorldLocation(markerPos);

		}
		else if (pawn->GetLocalRole() == ROLE_AutonomousProxy)
		{
			Server_MeshPosUpdate(this, markerPos);
		}
	}
	if (bspawn && menuSubsystemActor) DetectActivationMenuSystem();

	if (!focus_grabbed)
	{
		// Use a physics raycast to find vodgets in the scene.
		UVodget* hitVodget = DoRaycast();

		// If a vodgets is found, give it focus and begin forwarding any events while it still has focus.
		if (hitVodget != focusVodget)
		{
			// Let the previous focusVodgetCurr know that it has lost focus here.
			if (focusVodget != nullptr) {

				focusVodget->Focus(this, false);
			}

			focusVodget = hitVodget;

			// Let the focusVodgetCurr know that it now has selector focus here.
			if (focusVodget != nullptr) {

				focusVodget->Focus(this, true);

			}


		}

	}

	UpdatePalmTrackingPoint(DeltaTime); // This needs to always run to keep the current state of the hand updated or change magnitude for predictive movement
	if (handTrackingActive) // Hand tracking, when sight is lost hand tracking is disabled.
	{

		CheckHandGestures(DeltaTime); // checking for palm in motion or low motion

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
	currentHand = Cast<UMotionControllerComponent>(hand->GetAttachParent());
	leftOrRight = currentHand->GetTrackingSource();
	if (fireAffect) {
		fireAffect->AttachToComponent(hand, FAttachmentTransformRules::KeepRelativeTransform);
		fireAffect->SetWorldScale3D(FVector(0.2f));
		fireAffect->SetVariableLinearColor(FName("User.Color"), FLinearColor::Blue);
		fireAffect->ActivateSystem();
	}
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
		//UE_LOG(LogTemp, Warning, TEXT("SetMaterial called from server client"));
		Server_SetMaterial_Implementation(color);
	}
	else if (pawn->GetLocalRole() == ROLE_AutonomousProxy) {
		//UE_LOG(LogTemp, Warning, TEXT("SetMaterial called from remote client"));
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