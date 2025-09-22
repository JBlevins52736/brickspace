// Fill out your copyright notice in the Description page of Project Settings.


#include "BrickSpacePawn.h"
#include "HandSelector.h"
#include "AssemblyActor.h"
#include "WallBrick.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ABrickSpacePawn::ABrickSpacePawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;
	//PrimaryActorTick.bStartWithTickEnabled = true;

	SetReplicates(true);
}

void ABrickSpacePawn::VARLog(FString methodName)
{
	FString locstr = (IsLocallyControlled()) ? TEXT("LocallyControlled") : TEXT("NotLocallyControlled");
	switch (GetLocalRole())
	{
	case ROLE_Authority:		// Server Actor version: This is also the hosting listen client version.
		UE_LOG(LogTemp, Warning, TEXT("%s: ROLE_Authority: Hosting client running on server %s"), *methodName, *locstr );
		break;
	case ROLE_AutonomousProxy:	// Non-Server Player Actor version: IsLocallyControlled should be true
		UE_LOG(LogTemp, Warning, TEXT("%s: ROLE_AutonomousProxy: Player client running remotely %s"), *methodName, *locstr);
		break;
	case ROLE_SimulatedProxy:	// Non-Server Ghost
		UE_LOG(LogTemp, Warning, TEXT("%s: ROLE_SimulatedProxy: Ghost client running remotely %s"), *methodName, *locstr);
		break;
	}
}

void ABrickSpacePawn::Server_ChangeMaterial_Implementation(UBrick* brick, UMaterialInterface* material, bool brickIsSolid)
{
	brick->Server_ChangeMaterial_Implementation(material, brickIsSolid);
}

void ABrickSpacePawn::Server_Move_Implementation(AActor* TargetActor, const FTransform& InitialTransform)
{
	TargetActor->SetActorTransform(InitialTransform);
}


void ABrickSpacePawn::Server_Delete_Implementation(AActor* TargetActor)
{
	TargetActor->Destroy(true, true);
}

void ABrickSpacePawn::Server_CloneWallBrick_Implementation(UWallBrick* wallBrick, const FTransform& onWallTransform)
{
	wallBrick->Server_CloneWallBrick_Implementation(onWallTransform);

	// When we set this from the server it will replicate to all clients.
	//UWallBrick* wallBrick = clonedBrick->FindComponentByClass<UWallBrick>();
	//if (wallBrick != nullptr)
	//    wallBrick->bThresholdReached = true;
}


void ABrickSpacePawn::Server_TryAdvanceLayer_Implementation(AAssemblyActor* assemblyActor)
{
	if (!assemblyActor)
	{
		// Note: The client tests for null before calling this on the server.
		// When a valid pointer is passed in an RPC but arrives as null on the server the object referenced must not be replicated.
		// Actors passed as pointers in an RPC must be replicated to be deserialized (found) on the server.
		UE_LOG(LogTemp, Warning, TEXT("assemblyActor null in server TryAdvanceLayer call"));
	}
	else {
		UAssembly* assembly = assemblyActor->FindComponentByClass<UAssembly>();
		if (assembly)
			assembly->TryAdvanceLayer();
	}
}