// Fill out your copyright notice in the Description page of Project Settings.


#include "HandSelector.h"

#include "Vodget.h"
#include "Net/UnrealNetwork.h" // Required for DOREPLIFETIME
#include "BrickSpacePlayerState.h"
#include "GameFramework/Pawn.h"


UHandSelector::UHandSelector()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

UVodget* UHandSelector::DoRaycast()
{
	UVodget* retval = nullptr;

	// You can use this to customize various properties about the trace
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	FVector EyePos = centerEye->GetComponentLocation();
	FVector HandPos = hand->GetComponentLocation();

	FVector ray = HandPos - EyePos;
	ray.Normalize();
	ray *= 20.0;

	FVector StartPos = HandPos - ray;
	FVector EndPos = HandPos + ray;

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

// Called when the game starts
void UHandSelector::SetCursor()
{
	cursor = hand->GetComponentTransform();

	// Search for Vodget components when focus is not grabbed.
	if (focusVodget != nullptr) {
		if (focus_grabbed) {
			// Set cursor to the scaled world grab location when focusVodget exists and focus is grabbed.
			//FVector EyePos = head->GetComponentTransform().TransformPosition(dominantEyeOffset);
			FVector EyePos = centerEye->GetComponentLocation();
			FVector HandPos = hand->GetComponentLocation();

			// The current eye-hand dir.
			FVector eyeHandDir = (HandPos - EyePos);

			// Note: eyeHandDir is scaled by multiplying by ratio of initial hit-Eye over hand-eye distances
			FVector eyeHitVec = eyeHandDir * ratioHitEyeOverHandEye;

			cursor.SetLocation(EyePos + eyeHitVec);
		}
		else
		{
			// Set cursor to hit point when focusVodget exists.
			//FVector worldHitVec = cursor.TransformPosition(localHitVec);
			cursor.SetLocation(Hit.ImpactPoint);
		}
	}
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
			PrimaryComponentTick.SetTickFunctionEnable(false);
			return;
		}
	}

	//if ( !HasAuthority() ) {
	//	PrimaryComponentTick.SetTickFunctionEnable(false);
	//	return;
	//}

	if (hand == nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("Ticking but Hand is nullptr in HandSelector.cpp"));

		return;
	}

	Server_MeshPosUpdate_Implementation(hand->GetComponentLocation() );

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

void UHandSelector::OnRep_MeshPosUpdate()
{
	//FVector troubleShoot = handTransform.GetLocation();
	//GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Red, FString::Printf(TEXT("x: %f y:%f z:%f"),troubleShoot.X, troubleShoot.Y, troubleShoot.Z));

	//handMesh->SetWorldTransform(handTransform);

	APawn* pawn = Cast<APawn>(GetOwner());
	if (pawn && !pawn->IsLocallyControlled()) {
		if (handMesh)
			handMesh->SetWorldLocation(handPos);
		else UE_LOG(LogTemp, Error, TEXT("Hand mesh is nullptr in MeshPosUpdate. HandSelector.cpp"));
	}
}

void UHandSelector::Server_MeshPosUpdate_Implementation(UHandSelector*selector, FVector pos)
{
	selector->handPos = pos;
	selector->OnRep_MeshPosUpdate();
}


void UHandSelector::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UHandSelector, handPos);
}
