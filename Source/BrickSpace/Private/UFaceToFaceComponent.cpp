// Fill out your copyright notice in the Description page of Project Settings.


#include "UFaceToFaceComponent.h"
#include"Engine/Engine.h"


// Sets default values for this component's properties
UUFaceToFaceComponent::UUFaceToFaceComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UUFaceToFaceComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	Registry.Add(this);
	
}

void UUFaceToFaceComponent::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	Registry.Remove(this);
}


// Called every frame
void UUFaceToFaceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	AActor* Owner = GetOwner();

	for (UUFaceToFaceComponent* othercomp : Registry) {
		AActor* Other = othercomp->GetOwner();
		if (DetectEyeContact(Other)) {
			GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Green, "Bruh its working yay");
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Green, "Bruh its not working");
		}
	}

}

bool UUFaceToFaceComponent::DetectEyeContact(AActor* other)
{
	AActor* Owner = GetOwner();

	// Get Forward Vector
	 FVector Owner_F = Owner->GetActorForwardVector().GetSafeNormal();
	FVector Target_F = other->GetActorForwardVector().GetSafeNormal();

	//Get vector between 2 positions
	FVector OtoT = (Owner->GetActorLocation() - other->GetActorLocation()); //Owner to Target
	FVector TtoO = (other->GetActorLocation() - Owner->GetActorLocation()); // Target to Owner

	// Dot Product
	float DotA = FVector::DotProduct(Owner_F, OtoT);
	float DotB = FVector::DotProduct(Target_F, TtoO);
	float DotOpp = FVector::DotProduct(Owner_F, Target_F);

	float Threshold = FMath::Cos(FMath::DegreesToRadians(15.0f));

	//
	return (DotA>=Threshold) && (DotB >= Threshold) && (DotOpp >= Threshold);
}

