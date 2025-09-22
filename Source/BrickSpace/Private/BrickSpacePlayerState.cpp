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

	//AAssemblyActor* assemblyActor = nullptr;
	//if (groundplate) {
	//	TArray<AActor*> ChildActors;
	//	groundplate->GetAllChildActors(ChildActors, true); // true to include descendants
	//	for (AActor* ChildActor : ChildActors)
	//	{
	//		assemblyActor = Cast<AAssemblyActor>(ChildActor);
	//		if (assemblyActor)
	//		{
	//			assembly = assemblyActor->FindComponentByClass<UAssembly>();
	//			break; // Found the assemblyActor, no need to continue searching
	//		}
	//	}
	//}
}

