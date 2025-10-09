// Fill out your copyright notice in the Description page of Project Settings.


#include "WallLever.h"
#include "Components/SceneComponent.h"
#include "Selector.h"

UWallLever::UWallLever()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UWallLever::BeginPlay()
{
	Super::BeginPlay();
	
}

void UWallLever::Focus(USelector* cursor, bool state)
{
	UVodget::Focus(cursor, state);

}

void UWallLever::ForePinch(USelector* cursor, bool state)
{
	if (state) 
	{
		if (cursor && clientComponent)
		{
			grabbingSelector = cursor;

			FVector worldPos = grabbingSelector->Cursor().GetLocation();
			grabvec = clientComponent->GetComponentTransform().InverseTransformPosition(worldPos);
			grabvec.X = 0.0;

			cursor->GrabFocus(true);
			PrimaryComponentTick.SetTickFunctionEnable(true);
		}
	}
	else 
	{
		if (grabbingSelector)
		{
			grabbingSelector->GrabFocus(false);
			grabbingSelector = nullptr;
			PrimaryComponentTick.SetTickFunctionEnable(false); 
			FRotator snapBack = FRotator(0.0f, 0.0f, 0.0f);
			clientComponent->SetRelativeRotation(snapBack);
		}
	}
}


void UWallLever::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (grabbingSelector)
	{
		FVector worldPos = grabbingSelector->Cursor().GetLocation();
		FVector localPos = clientComponent->GetComponentTransform().InverseTransformPosition(worldPos);
		FVector currVec(0.0f, localPos.Y, localPos.Z);

		FQuat deltaRot = FQuat::FindBetweenVectors(grabvec, currVec);

		//UE_LOG(LogTemp, Warning, TEXT("Error: %.2f"), ds);

		FQuat currentRot = clientComponent->GetRelativeRotation().Quaternion();
		FQuat rawNewRotQuat =  deltaRot * currentRot;
		//clientComponent->SetRelativeRotation(rawNewRotQuat);


		FRotator rawNewRot = rawNewRotQuat.Rotator();
		float currentRoll = rawNewRot.Roll;
		float clampedRoll = FMath::Clamp(currentRoll, MinPitch, MaxPitch);
		FRotator finalRot = FRotator(0.0f, 0.0f, clampedRoll);
		clientComponent->SetRelativeRotation(finalRot);

		float pctSpeed = clampedRoll / MaxPitch;

		/*OnLeverMoved.Broadcast(clampedRoll);*/
		OnLeverMoved.Broadcast(pctSpeed);
	}
	else
	{
		
		PrimaryComponentTick.SetTickFunctionEnable(false);
		
	}
}