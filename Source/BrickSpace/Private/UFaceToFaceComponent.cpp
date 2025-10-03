// Fill out your copyright notice in the Description page of Project Settings.


#include "UFaceToFaceComponent.h"
#include"Engine/Engine.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h" // Include for ACharacter


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

	APawn* pawn = Cast<APawn>(GetOwner());
	if (pawn && pawn->IsLocallyControlled())
		SomebodyJoinedOrLeft();

	// ...
	Registry.Add(this);
}

void UUFaceToFaceComponent::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	Registry.Remove(this);
}

void UUFaceToFaceComponent::SomebodyJoinedOrLeft()
{
	// Clear the array
	Registry.Empty();

	APawn* pawn = Cast<APawn>(GetOwner());
	if (pawn && pawn->IsLocallyControlled())
	{
		// Update our local registry of all the other pawns.
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter::StaticClass(), FoundActors);

		// Now, FoundActors contains all ACharacter instances in the level.
		// You can iterate through this array and cast to ACharacter if needed.

		for (AActor* Actor : FoundActors)
		{

			// Find the UFaceToFaceComponent AND if its NOT this (you) add it.
			if (!Actor) continue;
			if (UUFaceToFaceComponent* Comp = Actor->FindComponentByClass<UUFaceToFaceComponent>())
			{
				if (Comp != this)
					Registry.Add(Comp);
			}

			//ACharacter* Character = Cast<ACharacter>(Actor);
			//if (Character)
			//{
			//	// Do something with the Character, e.g., access properties or call functions
			//	// UE_LOG(LogTemp, Warning, TEXT("Found character: %s"), *Character->GetName());


			//}
		}
	}
}


// Called every frame
void UUFaceToFaceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	AActor* Owner = GetOwner();
	FVector Start, Forward;
	Start = GetComponentLocation();
	Forward = GetForwardVector();


	// Draw debug line 
	FVector End = Start + Forward * 1000.0f;
	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 0, 0, 2.0f);
	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 0, 0, 2.0f);


	for (UUFaceToFaceComponent* othercomp : Registry) {
		//AActor* Other = othercomp->GetOwner();
		if (DetectEyeContact(othercomp)) {
			//Other->overlay;
			GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Green, "Bruh its  working");
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Green, "Bruh its not working");
		}
	}

}

bool UUFaceToFaceComponent::DetectEyeContact(UUFaceToFaceComponent* other)
{
	// Get Forward Vector
	FVector Owner_F = GetForwardVector();
	FVector Target_F = other->GetForwardVector();

	//Get vector between 2 positions
	FVector mypos = GetComponentLocation();
	FVector othpos = other->GetComponentLocation();

	FVector OtoT = (othpos - mypos).GetSafeNormal(); //Owner to Target
	FVector TtoO = (mypos - othpos).GetSafeNormal(); // Target to Owner

	// Dot Product
	float DotA = FVector::DotProduct(Owner_F, OtoT);
	float DotB = FVector::DotProduct(Target_F, TtoO);
	float DotOpp = FVector::DotProduct(Owner_F, Target_F);

	FString dota = "othpos:" + FString::SanitizeFloat(othpos.X) + "," + FString::SanitizeFloat(othpos.Y) + "," + FString::SanitizeFloat(othpos.Z);
	FString dotb = "mypos" + FString::SanitizeFloat(mypos.X) + "," + FString::SanitizeFloat(mypos.Y) + "," + FString::SanitizeFloat(mypos.Z);
	FString dotopp = "mydir:" + FString::SanitizeFloat(Owner_F.X) + "," + FString::SanitizeFloat(Owner_F.Y) + "," + FString::SanitizeFloat(Owner_F.Z);
	FString dototot = "otot:" + FString::SanitizeFloat(OtoT.X) + "," + FString::SanitizeFloat(OtoT.Y) + "," + FString::SanitizeFloat(OtoT.Z);

	//GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Green,dota,true );
	//GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Green, dotb,true);
	//GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Green, dotopp,true);
	//GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Green, dototot, true);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *dota);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *dotb);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *dotopp);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *dototot);

	//float Threshold = FMath::Cos(FMath::DegreesToRadians(90.0f));
	float Threshold = FMath::Cos(PI / 2.0f);


	//bool ISFaceToFace = (DotA >= Threshold) && (DotB >= Threshold) && (-DotOpp >= Threshold);

	bool ISFaceToFace = (DotA >= Threshold);
	return ISFaceToFace;

	//AActor* Owner = GetOwner();


	//FVector OwnerPos, OwnerFwd;
	//if (UCameraComponent* Cam = Owner->FindComponentByClass<UCameraComponent>())
	//{
	//	OwnerPos = Cam->GetComponentLocation();	
	//	OwnerFwd = Cam->GetForwardVector().GetSafeNormal();
	//}
	//else
	//{
	//	OwnerPos = Owner->GetActorLocation();
	//	OwnerFwd = Owner->GetActorForwardVector().GetSafeNormal();
	//}

	//FVector OtherPos, OtherFwd;
	//if (UCameraComponent* OtherCam = other->FindComponentByClass<UCameraComponent>())
	//{
	//	OtherPos = OtherCam->GetComponentLocation();
	//	OtherFwd = OtherCam->GetForwardVector().GetSafeNormal();
	//}
	//else
	//{
	//	OtherPos = other->GetActorLocation();
	//	OtherFwd = other->GetActorForwardVector().GetSafeNormal();
	//}

	//// Vectors between them
	//FVector OwnerToOther = (OtherPos - OwnerPos).GetSafeNormal();
	//FVector OtherToOwner = -OwnerToOther;

	//// Dot products
	//float DotA = FVector::DotProduct(OwnerFwd, OwnerToOther);
	//float DotB = FVector::DotProduct(OtherFwd, OtherToOwner);
	//float DotOpp = FVector::DotProduct(OwnerFwd, OtherFwd);

	//// Threshold 
	//float Threshold = FMath::Cos(FMath::DegreesToRadians(90.0f));

	//      
}

