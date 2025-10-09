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

	APawn* PawnOwner = Cast<APawn>(GetOwner());
	if (PawnOwner && PawnOwner->IsLocallyControlled())
	{     
		SomebodyJoinedOrLeft();
	}
	Registry.Add(this);

	// ...
}

void UUFaceToFaceComponent::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	Registry.Remove(this);
	// Registry.Add(this);
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
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), FoundActors);

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
		}
	}
}

void UUFaceToFaceComponent::BrickTransfer(UUFaceToFaceComponent* Other)
{
	if (FindBrick()) {
		AActor* R_Brick = RBrick->GetOwner();
		Transfer.P0 = R_Brick->GetActorLocation();

		const FVector otherPos = Other->GetComponentLocation();
		const FVector fwd = Other->GetForwardVector();
		const float FrontOffset = 70.f;  
		const float HoldHeight = 25.f; 

		Transfer.P1 = otherPos + fwd * FrontOffset + FVector::UpVector * HoldHeight;
		Transfer.Elapsed = 0.f;
		Transfer.Duration = 0.7f;  
		Transfer.ArcHeight = 60.f;
		Transfer.bActive = true;
	}
	
}

void UUFaceToFaceComponent::LerpBrick(float DeltaTime)
{
	AActor* R_Brick = RBrick->GetOwner();

	Transfer.Elapsed += DeltaTime;
	 float s = FMath::Clamp(Transfer.Elapsed / Transfer.Duration, 0.f, 1.f);

	 FVector base = FMath::Lerp(Transfer.P0, Transfer.P1, s);
	 const float   bump = 4.f * s * (1.f - s) * Transfer.ArcHeight; 
	 const FVector pos = base + FVector::UpVector * bump;
	 R_Brick->SetActorLocation(pos);

	 const FVector dir = (Transfer.P1 - Transfer.P0).GetSafeNormal();
	 if (!dir.IsNearlyZero())
	 {
		 R_Brick->SetActorRotation(dir.Rotation());
	 }

	 if (s >= 1.f)
	 {
		 
		 R_Brick->SetActorLocation(Transfer.P1);
	 }

}

bool UUFaceToFaceComponent::FindBrick()
{
	bool FoundBrick = false;
	TArray<UActorComponent*> SceneComps;
	AActor* Owner = GetOwner();
	Owner->GetComponents<UActorComponent>(SceneComps);
	//GetOwner()->GetAllChildActors(Actorchild, true);
	for (UActorComponent* Selector : SceneComps) {
		if (USelector* SelectorComp = Cast<USelector>(Selector))
		{
			const FName CompName = SelectorComp->GetFName();
			/*if (CompName == "HandSelectorL") {
				if (LBrick = SelectorComp->GetFocusGrabbedVodget())
				{
					FoundBrick = true;
					UE_LOG(LogTemp, Warning, TEXT("Found brick: %s"), *LBrick->GetName());
				}
			}*/
			if (CompName == "HandSelectorR") {
				RBrick = SelectorComp->GetFocusGrabbedVodget();
				if (RBrick != nullptr)
				{
					FoundBrick = true;
					UE_LOG(LogTemp, Warning, TEXT("Found brick: %s"), *RBrick->GetName());
				}if (RBrick == nullptr) {
					FoundBrick = false;
				}
				break;
			}
			
		}

	}
	return FoundBrick;
	
}


// Called every frame
void UUFaceToFaceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	//AActor* Owner = GetOwner();
	FVector Start, Forward;
	Start = GetComponentLocation();
	Forward = GetForwardVector();

	// Draw debug line 
	FVector End = Start + Forward * 1000.0f;
	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 0, 0, 2.0f);
	


	for (UUFaceToFaceComponent* othercomp : Registry) {
		//AActor* Other = othercomp->GetOwner();
		if (othercomp != this) {
			FString phrase = othercomp->GetOwner()->GetName();
			float EyeContact = DetectEyeContactHeld(othercomp,DeltaTime);
			if (EyeContact >= HoldSeconds) {
				ShowFaceWidget(true);
				GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Green, "Bruh its  working (held >= 0.7s)");
			}
			 if (!Transfer.bActive && EyeContact >= TransferActive) {
				BrickTransfer(othercomp);
				
			}
			else {
				ShowFaceWidget(false);
				 float* TimerPtr = EyeContactTimers.Find(othercomp);
				float t = TimerPtr ? *TimerPtr : 0.f;
				FString Msg = "Maintaining eye contact: %.2fs / %.2fs" + FString::SanitizeFloat(t, HoldSeconds);
				GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Yellow, Msg);
			}
		}
	}
	LerpBrick(DeltaTime);
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
	FString Name = GetOwner()->GetName();
	FString dota = "othpos:" + FString::SanitizeFloat(othpos.X) + "," + FString::SanitizeFloat(othpos.Y) + "," + FString::SanitizeFloat(othpos.Z);
	FString dotb = "mypos" + FString::SanitizeFloat(mypos.X) + "," + FString::SanitizeFloat(mypos.Y) + "," + FString::SanitizeFloat(mypos.Z);
	FString dotopp = "mydir:" + FString::SanitizeFloat(Owner_F.X) + "," + FString::SanitizeFloat(Owner_F.Y) + "," + FString::SanitizeFloat(Owner_F.Z);
	FString dototot = "otot:" + FString::SanitizeFloat(OtoT.X) + "," + FString::SanitizeFloat(OtoT.Y) + "," + FString::SanitizeFloat(OtoT.Z);

	//GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Green,dota,true );
	//GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Green, dotb,true);
	//GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Green, dotopp,true);
	//GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Green, dototot, true);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Name);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *dota);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *dotb);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *dotopp);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *dototot);

	//float Threshold = FMath::Cos(FMath::DegreesToRadians(90.0f));
	
	float CosThresh = FMath::Cos(FMath::DegreesToRadians(AngleThreshold));

	bool ISFaceToFace = (DotA >= CosThresh) && (DotB >= CosThresh) && (DotOpp <= -CosThresh);

	//bool ISFaceToFace = (DotA >= Threshold);
	return ISFaceToFace;
}


float UUFaceToFaceComponent::DetectEyeContactHeld(UUFaceToFaceComponent* other, float DeltaTime)
{
	if (!other) return false;

	 bool bRaw = DetectEyeContact(other); 

	float& Timer = EyeContactTimers.FindOrAdd(other);
	Timer = bRaw ? (Timer + DeltaTime) : 0.f;

	return (Timer);
}

void UUFaceToFaceComponent::ShowFaceWidget(bool bShow)
{
	if (!FaceWidgetClass) return;

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return;

	APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController());
	if (!PC) return;

	if (bShow)
	{
		if (!ActiveWidget)
		{
			ActiveWidget = CreateWidget<UUserWidget>(PC, FaceWidgetClass);
			if (ActiveWidget)
			{
				ActiveWidget->AddToViewport();
			}
		}
	}
	else
	{
		if (ActiveWidget)
		{
			ActiveWidget->RemoveFromParent();
			ActiveWidget = nullptr;
		}
	}
}










