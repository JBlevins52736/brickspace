// Fill out your copyright notice in the Description page of Project Settings.

#include "BrickSpacePlayerState.h"
#include "HandSelector.h"
#include "WallBrick.h"
#include "AssemblyActor.h"
#include "Assembly.h"
#include "Net/UnrealNetwork.h"

void ABrickSpacePlayerState::BeginPlay()
{
	Super::BeginPlay();

	AAssemblyActor* assemblyActor = nullptr;
	if (groundplate) {
		TArray<AActor*> ChildActors;
		groundplate->GetAllChildActors(ChildActors, true); // true to include descendants
		for (AActor* ChildActor : ChildActors)
		{
			assemblyActor = Cast<AAssemblyActor>(ChildActor);
			if (assemblyActor)
			{
				assembly = assemblyActor->FindComponentByClass<UAssembly>();
				break; // Found the assemblyActor, no need to continue searching
			}
		}
	}


	//// Set replicated state of all bricks on the wall.
	//ENetMode CurrentNetMode = GetNetMode();
	//if (wallOfBricks && (CurrentNetMode == NM_ListenServer || CurrentNetMode == NM_Standalone)) {

	//	TArray<UWallBrick*> WallBricks;
	//	wallOfBricks->GetComponents<UWallBrick>(WallBricks);
	//	for (UWallBrick* wallBrick : WallBricks)
	//	{
	//		//wallBrick->assemblyActor = assemblyActor;
	//		//wallBrick->solidMatchMaterial = wallBrick->GetMaterial();
	//		//wallBrick->isSolid = true;
	//		//wallBrick->isGrabbable = true;
	//		//wallBrick->brickMaterial = wallBrick->solidMatchMaterial;
	//		//wallBrick->OnRep_Material();
	//		//wallBrick->OnRep_Grabbable();
	//		//wallBrick->OnRep_Parent();
	//	}
	//}

}

void ABrickSpacePlayerState::Server_Own_Implementation(AActor* TargetActor, AActor* pawn)
{
	FString resultString;
	TargetActor->GetName(resultString);
	UE_LOG(LogTemp, Warning, TEXT("Server_Own_Implementation TargetActor:%s"), *resultString);
	TargetActor->SetOwner(pawn);
}

//void ABrickSpacePlayerState::Server_CloneActor_Implementation(AActor* TargetActor, const FTransform& InitialTransform)
//{
//	AActor* Replacement = GetWorld()->SpawnActor<AActor>(
//		TargetActor->GetClass(),
//		InitialTransform
//	);
//
//	UWallBrick* wallBrick = TargetActor->FindComponentByClass<UWallBrick>();
//	if (wallBrick != nullptr)
//		wallBrick->bThresholdReached = true;
//}

//void ABrickSpacePlayerState::Server_MoveActor_Implementation(AActor* TargetActor, const FTransform& InitialTransform)
//{
//	//clientComponent->SetWorldTransform(worldsrt);
//	TargetActor->SetActorTransform(InitialTransform);
//}

//void ABrickSpacePlayerState::Server_DeleteActor_Implementation(AActor* TargetActor)
//{
//	TargetActor->Destroy(true, true);
//}

void ABrickSpacePlayerState::Server_ChangeMaterial_Implementation(AActor* TargetActor, UMaterialInterface* material, bool brickIsSolid)
{
	UBrick* brick = TargetActor->FindComponentByClass<UBrick>();
	if (brick != nullptr) {
		brick->brickMaterial = material;
		brick->isSolid = brickIsSolid;
		brick->OnRep_Material();
	}
}

void ABrickSpacePlayerState::Server_TryAdvanceLayer_Implementation(AAssemblyActor* assemblyActor)
{
	//ENetMode CurrentNetMode = GetNetMode();
	//if (CurrentNetMode != NM_ListenServer)
	//{
	//	// This instance is not the server
	//	return;
	//}

	//UAssembly* assembly = GroundplateActor->FindComponentByClass<UAssembly>();
	//if (assembly == nullptr) {
	//	if (groundplate) {
	//		TArray<AActor*> ChildActors;
	//		groundplate->GetAllChildActors(ChildActors, true); // true to include descendants
	//		for (AActor* ChildActor : ChildActors)
	//		{
	//			AAssemblyActor* assemblyActor = Cast<AAssemblyActor>(ChildActor);
	//			if (assemblyActor)
	//			{
	//				assembly = assemblyActor->FindComponentByClass<UAssembly>();
	//				break; // Found the assemblyActor, no need to continue searching
	//			}
	//		}
	//	}
	//}

	if (!assemblyActor)
	{
		// Note: The client tests for null before calling this on the server.
		// When a valid pointer is passed in an RPC but arrives as null on the server the object referenced must not be replicated.
		// Actors passed as pointers in an RPC must be replicated to be deserialized (found) on the server.
		UE_LOG(LogTemp, Warning, TEXT("assemblyActor null in server TryAdvanceLayer call"));
	}
	else {
		if (!assembly)
			assembly = assemblyActor->FindComponentByClass<UAssembly>();
		if (assembly)
			assembly->TryAdvanceLayer();
	}
}

void ABrickSpacePlayerState::Server_ChangeHandColor_Implementation(AActor* target, UMaterialInterface* material)
{
	TArray<UActorComponent*> actorComp;
	target->GetComponents(actorComp);
	// this will need adjustment because its going to look for the first one and that is it
	for (UActorComponent* actor : actorComp) {
		UHandSelector* selector = Cast<UHandSelector>(actor);
		if (selector) {
			selector->handMaterial = material;
			selector->OnRep_Material();
			break;
		}
	}
}
//
//void ABrickSpacePlayerState::Server_UpdatePlayerHandPos_Implementation(AActor* target, FTransform leftTransform, FTransform rightTransform)
//{
//	TArray<UActorComponent*> actorComp;
//	target->GetComponents(actorComp);
//	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Orange, TEXT("Server was notified"));
//	UE_LOG(LogTemp, Error, TEXT("=== SERVER RPC RECEIVED ==="));
//	for (UActorComponent* actor : actorComp)
//	{
//		if (actor->GetName().Contains("HandSelectorL"))
//		{
//			UHandSelector* handSelector = Cast<UHandSelector>(actor);
//			handSelector->handTransform = leftTransform;
//
//		}
//		else if (actor->GetName().Contains("HandSelectorR"))
//		{
//			UHandSelector* handSelector = Cast<UHandSelector>(actor);
//			handSelector->handTransform = rightTransform;
//
//		}
//	}
//}
