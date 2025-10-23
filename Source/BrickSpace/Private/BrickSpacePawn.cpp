// Fill out your copyright notice in the Description page of Project Settings.


#include "BrickSpacePawn.h"
#include "HandSelector.h"
#include "AssemblyActor.h"
#include "Assembly.h"
#include "WallBrick.h"
#include "Net/UnrealNetwork.h"
#include "TimeManager.h"
#include "WallMover.h"

// Sets default values
ABrickSpacePawn::ABrickSpacePawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;
	//PrimaryActorTick.bStartWithTickEnabled = true;

	SetReplicates(true);
}

void ABrickSpacePawn::ActivateParticleSystem_Implementation(bool isActive)
{
	
	TArray<UActorComponent*> components = K2_GetComponentsByClass(UHandSelector::StaticClass());
	
	
	
	for (UActorComponent* child : components)
	{
		if (UHandSelector* selector = Cast<UHandSelector>(child)) {
			selector->StartStopParticleSystem(isActive);
		}
	}

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

void ABrickSpacePawn::Server_MoveRelative_Implementation(USceneComponent* TargetActor, const FTransform& InitialTransform)
{
	TargetActor->SetRelativeTransform(InitialTransform);
}

void ABrickSpacePawn::Server_Translate_Implementation(UWallMover* TargetActor, float pct)
{
	TargetActor->SetMovementTarget(pct);

}

void ABrickSpacePawn::Server_Rotate_Implementation(USceneComponent* TargetActor, const FRotator& Rot)
{
	TargetActor->SetRelativeRotation(Rot);
}

void ABrickSpacePawn::Server_Delete_Implementation(AActor* TargetActor)
{
	TargetActor->Destroy(true, true);
}

void ABrickSpacePawn::Server_CloneWallBrick_Implementation(UWallBrick* wallBrick, const FTransform& onWallTransform)
{
	wallBrick->CloneWallBrick(onWallTransform);
}

void ABrickSpacePawn::Server_TryAdvanceLayer_Implementation(UBrick* assemblyBrick)
{
	AAssemblyActor* assemblyActor = assemblyBrick->assemblyActor;
	UAssembly* assembly = assemblyActor->FindComponentByClass<UAssembly>();
	if (assembly)
		assembly->TryAdvanceLayer();
}

void ABrickSpacePawn::Server_MeshScaleUpdate_Implementation(USceneComponent* leftHandMesh, USceneComponent* rightHandMesh, float handScale)
{
	FVector scaleVec = FVector::OneVector * handScale;
	leftHandMesh->SetWorldScale3D(scaleVec);
	rightHandMesh->SetWorldScale3D(scaleVec);
}
void ABrickSpacePawn::Server_StartStopTimer_Implementation(UTimeManager* timeManager, bool isRunning)
{
	if (!timeManager) return;

	// 1. Set the authoritative state (replicated to all)
	timeManager->bIsRunning = isRunning;

	// 2. Issue the immediate command to all clients (Multicast)
	if (isRunning)
	{
		timeManager->Client_StartTimer();
	}
	else
	{
		timeManager->Client_StopTimer();
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Server_StartStopTimer: Command issued. bIsRunning=%d"), timeManager->bIsRunning);
}


void ABrickSpacePawn::Server_ResetTimer_Implementation(UTimeManager* timeManager)
{
	if (!timeManager) return;

	timeManager->ResetTimer(this);

	UE_LOG(LogTemp, Warning, TEXT("Server_ResetTimer: Reset command issued."));
}

void ABrickSpacePawn::Server_HandleLaunchButtonPress_Implementation(USliderButton* ButtonComponent)
{

	if (ButtonComponent && ButtonComponent->timer)
	{

		ButtonComponent->timer->StopTimer(this);
		ButtonComponent->Press();
	}
}

