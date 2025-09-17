// Fill out your copyright notice in the Description page of Project Settings.


#include "BrickSpacePawn.h"
#include <Kismet/GameplayStatics.h>
#include "HandSelector.h"
#include "Net/UnrealNetwork.h"
// Sets default values
ABrickSpacePawn::ABrickSpacePawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);
	PrimaryActorTick.bStartWithTickEnabled = true;

	PrimaryActorTick.TickInterval = delayInterval;
}

// Called when the game starts or when spawned
void ABrickSpacePawn::BeginPlay()
{
	Super::BeginPlay();
	if (APlayerController* PC = Cast<APlayerController>(GetController())) {
		playerState = Cast<ABrickSpacePlayerState>(PC->PlayerState);
		//if(!playerState) UE_LOG(LogTemp, Error, TEXT("Failed to acquire player state"))
	}
	SetActorTickEnabled(true);

	//UE_LOG(LogTemp, Warning, TEXT("Begin play has finished"));
}

void ABrickSpacePawn::NotifyServerOfHandMatChange(USelector* selector, UMaterialInterface* material)
{
	if (playerState)
		playerState->Server_ChangeHandColor(this, material);

}

void ABrickSpacePawn::UpdateHandColor(UMaterialInterface* color, USelector* selector)
{
	// This function checks to see who is the owner and will handle the calls from there.
	// This function is managing updates between clients and servers as displayed in the Play in Editor
	if (IsLocallyControlled()) {
		if (HasAuthority()) {
			UE_LOG(LogTemp, Warning, TEXT("I am server and updating my hand color"));

			//UHandSelector* handSelector = Cast<UHandSelector>(selector);
			//handSelector->handMaterial = color;
			//handSelector->OnRep_Material(); // For some reason, this is necessary here, but not when I update hand position.
			
			// LocallyControlled pawn with authority needs to make this change on the server to have changes propogate to all clients.
			// Calling OnRep_Material() is supposed to be called from the server.
			// While this doesn't change the local copy immediately, OnRep_Material() will be called eventually.
			// Another option would be to change the color locally first and use LocallyControlled() inside OnRep_Material to avoid calling it again.
			ServerUpdatePlayerHandColor(this, color, selector);
		}
		else
		{
			// LocallyControlled pawns always have authority so this case shouldn't be possible.
			UE_LOG(LogTemp, Warning, TEXT("I am a client updating hand color"));
			//ServerUpdatePlayerHandColor(this, color, selector);
		}
	}
}

// Called every frame
void ABrickSpacePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsLocallyControlled())
	{
		ServerUpdatePlayerHandPos(this, leftHand->GetComponentLocation(), rightHand->GetComponentLocation() );
	}
#ifdef BLAH
	elapsedTickTime += DeltaTime;
	//UE_LOG(LogTemp, Warning, TEXT("Tick is firing on the clients"));
	if (elapsedTickTime > delayInterval)
	{
		TArray<UActorComponent*> components;
		this->GetComponents<UActorComponent>(components);
		FTransform rightTrans;
		FTransform leftTrans;
		UHandSelector* right = nullptr;
		UHandSelector* left = nullptr;
		bool foundComp = false;
		for (UActorComponent* component : components)
		{
			if (component->GetName().Contains("HandSelectorL"))
			{
				UHandSelector* handSelector = Cast<UHandSelector>(component);
				leftTrans = handSelector->handMesh->GetComponentTransform();
				left = handSelector;

			}
			else if (component->GetName().Contains("HandSelectorR"))
			{
				UHandSelector* handSelector = Cast<UHandSelector>(component);
				rightTrans = handSelector->handMesh->GetComponentTransform();
				right = handSelector;
			}
		}


		if (IsLocallyControlled() && HasAuthority()) {
			left->handTransform = leftTrans;
			right->handTransform = rightTrans;
		}
		else if (IsLocallyControlled() && !HasAuthority())
		{
			//UE_LOG(LogTemp, Warning, TEXT("Client sending update"));

			ServerUpdatePlayerHandPos(this, leftTrans, rightTrans);
		}
		elapsedTickTime -= delayInterval;
	}
#endif
}

void ABrickSpacePawn::ServerUpdatePlayerHandPos_Implementation(AActor* target, FVector left, FVector right)
{
	if ( !IsLocallyControlled() )
	{
		leftHand->SetWorldLocation(left);
		rightHand->SetWorldLocation(right);
	}
#ifdef BLAH
	TArray<UActorComponent*> actorComp;
	target->GetComponents(actorComp);
	//UE_LOG(LogTemp, Error, TEXT("=== SERVER RPC RECEIVED ==="));
	for (UActorComponent* actor : actorComp)
	{
		if (actor->GetName().Contains("HandSelectorL"))
		{
			UHandSelector* handSelector = Cast<UHandSelector>(actor);
			handSelector->handTransform = left;
			handSelector->OnRep_MeshTransformUpdate();

		}
		else if (actor->GetName().Contains("HandSelectorR"))
		{
			UHandSelector* handSelector = Cast<UHandSelector>(actor);
			handSelector->handTransform = right;
			handSelector->OnRep_MeshTransformUpdate();
		}
	}
#endif
}

// Called to bind functionality to input
void ABrickSpacePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABrickSpacePawn::ServerUpdatePlayerHandColor_Implementation(AActor* target, UMaterialInterface* color, USelector* selector)
{
	if (selector) {
		UHandSelector* handSelector = Cast<UHandSelector>(selector);

		handSelector->handMaterial = color;
		handSelector->OnRep_Material();
	}
}



