// Fill out your copyright notice in the Description page of Project Settings.


#include "BrickSpacePawn.h"
#include <Kismet/GameplayStatics.h>
#include "HandSelector.h"
#include "Net/UnrealNetwork.h"
// Sets default values
ABrickSpacePawn::ABrickSpacePawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;
	//PrimaryActorTick.bStartWithTickEnabled = true;

	SetReplicates(true);
}

// Called when the game starts or when spawned
//void ABrickSpacePawn::BeginPlay()
//{
//	Super::BeginPlay();
//	if (APlayerController* PC = Cast<APlayerController>(GetController())) {
//		playerState = Cast<ABrickSpacePlayerState>(PC->PlayerState);
//		//if(!playerState) UE_LOG(LogTemp, Error, TEXT("Failed to acquire player state"))
//	}
//	SetActorTickEnabled(true);
//
//	//UE_LOG(LogTemp, Warning, TEXT("Begin play has finished"));
//}
//
//void ABrickSpacePawn::NotifyServerOfHandMatChange(USelector* selector, UMaterialInterface* material)
//{
//	if (playerState)
//		playerState->Server_ChangeHandColor(this, material);
//
//}
//
//void ABrickSpacePawn::VARLog(FString methodName)
//{
//	FString locstr = (IsLocallyControlled()) ? TEXT("LocallyControlled") : TEXT("NotLocallyControlled");
//	switch (GetLocalRole())
//	{
//	case ROLE_Authority:		// Server Actor version: This is also the hosting listen client version.
//		UE_LOG(LogTemp, Warning, TEXT("%s: ROLE_Authority: Hosting client running on server %s"), *methodName, *locstr );
//		break;
//	case ROLE_AutonomousProxy:	// Non-Server Player Actor version: IsLocallyControlled should be true
//		UE_LOG(LogTemp, Warning, TEXT("%s: ROLE_AutonomousProxy: Player client running remotely %s"), *methodName, *locstr);
//		break;
//	case ROLE_SimulatedProxy:	// Non-Server Ghost
//		UE_LOG(LogTemp, Warning, TEXT("%s: ROLE_SimulatedProxy: Ghost client running remotely %s"), *methodName, *locstr);
//		break;
//	}
//}

//void ABrickSpacePawn::UpdateHandColor(UMaterialInterface* color, USelector* selector)
//{
//	// This function checks to see who is the owner and will handle the calls from there.
//	// This function is managing updates between clients and servers as displayed in the Play in Editor
//
//	Log(TEXT("ABrickSpacePawn::UpdateHandColor"));
//
//	if (IsLocallyControlled()) {
//		if (HasAuthority()) {
//			ServerUpdatePlayerHandColor_Implementation(this, color, selector);
//		}
//		else if (GetLocalRole() == ROLE_AutonomousProxy) {
//			ServerUpdatePlayerHandColor(this, color, selector);
//		}
//
//		//if (!HasAuthority()) {
//		//	UE_LOG(LogTemp, Warning, TEXT("I am server and updating my hand color"));
//
//		//	//UHandSelector* handSelector = Cast<UHandSelector>(selector);
//		//	//handSelector->handMaterial = color;
//		//	//handSelector->OnRep_Material(); // For some reason, this is necessary here, but not when I update hand position.
//		//	
//		//	// LocallyControlled pawn with authority needs to make this change on the server to have changes propogate to all clients.
//		//	// Calling OnRep_Material() is supposed to be called from the server.
//		//	// While this doesn't change the local copy immediately, OnRep_Material() will be called eventually.
//		//	// Another option would be to change the color locally first and use LocallyControlled() inside OnRep_Material to avoid calling it again.
//		//	ServerUpdatePlayerHandColor(this, color, selector);
//		//}
//		//else
//		//{
//		//	// LocallyControlled pawns always have authority so this case shouldn't be possible.
//		//	UE_LOG(LogTemp, Warning, TEXT("I am a client updating hand color"));
//		//	//ServerUpdatePlayerHandColor(this, color, selector);
//		//}
//	}
//}

//void ABrickSpacePawn::ServerUpdatePlayerHandColor_Implementation(AActor* target, UMaterialInterface* color, USelector* selector)
//{
//	Log(TEXT("ABrickSpacePawn::ServerUpdatePlayerHandColor_Implementation"));
//
//	if (selector) {
//		UHandSelector* handSelector = Cast<UHandSelector>(selector);
//
//		handSelector->handMaterial = color;
//		handSelector->OnRep_Material();
//	}
//}

//void ABrickSpacePawn::UpdateHandPos(USelector* selector, FVector pos)
//{
//	if (IsLocallyControlled()) {
//		if (!HasAuthority()) {
//			UE_LOG(LogTemp, Warning, TEXT("I am server and UpdateHandPos"));
//			Server_MeshPosUpdate(GetOwner(), selector, pos);
//		}
//		else
//		{
//			// LocallyControlled pawns always have authority so this case shouldn't be possible.
//			//UE_LOG(LogTemp, Warning, TEXT("I am a client UpdateHandPos"));
//		}
//	}
//}

//void ABrickSpacePawn::Server_MeshPosUpdate_Implementation(AActor* target, USelector* selector, FVector pos)
//{
//	Log(TEXT("ABrickSpacePawn::Server_MeshPosUpdate_Implementation"));
//
//	if (selector) {
//		UHandSelector* handSelector = Cast<UHandSelector>(selector);
//
//		handSelector->handPos = pos;
//		handSelector->OnRep_MeshPosUpdate();
//	}
//}

//
//// Called every frame
//void ABrickSpacePawn::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//
//	ServerUpdatePlayerHandPos(this, leftHand->GetComponentLocation(), rightHand->GetComponentLocation() );
//	
//	elapsedTickTime += DeltaTime;
//	//UE_LOG(LogTemp, Warning, TEXT("Tick is firing on the clients"));
//	if (elapsedTickTime > delayInterval)
//	{
//		TArray<UActorComponent*> components;
//		this->GetComponents<UActorComponent>(components);
//		FTransform rightTrans;
//		FTransform leftTrans;
//		UHandSelector* right = nullptr;
//		UHandSelector* left = nullptr;
//		bool foundComp = false;
//		for (UActorComponent* component : components)
//		{
//			if (component->GetName().Contains("HandSelectorL"))
//			{
//				UHandSelector* handSelector = Cast<UHandSelector>(component);
//				leftTrans = handSelector->handMesh->GetComponentTransform();
//				left = handSelector;
//
//			}
//			else if (component->GetName().Contains("HandSelectorR"))
//			{
//				UHandSelector* handSelector = Cast<UHandSelector>(component);
//				rightTrans = handSelector->handMesh->GetComponentTransform();
//				right = handSelector;
//			}
//		}
//
//
//		if (IsLocallyControlled() && HasAuthority()) {
//			left->handTransform = leftTrans;
//			right->handTransform = rightTrans;
//		}
//		else if (IsLocallyControlled() && !HasAuthority())
//		{
//			//UE_LOG(LogTemp, Warning, TEXT("Client sending update"));
//
//			ServerUpdatePlayerHandPos(this, leftTrans, rightTrans);
//		}
//		elapsedTickTime -= delayInterval;
//	}
//}

//void ABrickSpacePawn::ServerUpdatePlayerHandPos_Implementation(AActor* target, FVector left, FVector right)
//{
//	if ( !IsLocallyControlled() )
//	{
//		//leftHand->SetWorldLocation(left);
//		//rightHand->SetWorldLocation(right);
//
//		if (leftSelector == nullptr || rightSelector == nullptr) {
//			TArray<UActorComponent*> actorComp;
//			target->GetComponents(actorComp);
//			//UE_LOG(LogTemp, Error, TEXT("=== SERVER RPC RECEIVED ==="));
//			for (UActorComponent* actor : actorComp)
//			{
//				if (actor->GetName().Contains("HandSelectorL"))
//				{
//					leftSelector = Cast<UHandSelector>(actor);
//				}
//				else if (actor->GetName().Contains("HandSelectorR"))
//				{
//					rightSelector = Cast<UHandSelector>(actor);
//				}
//			}
//		}
//
//		if (leftSelector) {
//			leftSelector->handPos = left;
//			leftSelector->OnRep_MeshPosUpdate();
//		}
//		if (rightSelector) {
//			rightSelector->handPos = left;
//			rightSelector->OnRep_MeshPosUpdate();
//		}
//	}
//	TArray<UActorComponent*> actorComp;
//	target->GetComponents(actorComp);
//	//UE_LOG(LogTemp, Error, TEXT("=== SERVER RPC RECEIVED ==="));
//	for (UActorComponent* actor : actorComp)
//	{
//		if (actor->GetName().Contains("HandSelectorL"))
//		{
//			UHandSelector* handSelector = Cast<UHandSelector>(actor);
//			handSelector->handTransform = left;
//			handSelector->OnRep_MeshTransformUpdate();
//
//		}
//		else if (actor->GetName().Contains("HandSelectorR"))
//		{
//			UHandSelector* handSelector = Cast<UHandSelector>(actor);
//			handSelector->handTransform = right;
//			handSelector->OnRep_MeshTransformUpdate();
//		}
//	}
//}


//// Called to bind functionality to input
//void ABrickSpacePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
//{
//	Super::SetupPlayerInputComponent(PlayerInputComponent);
//}



