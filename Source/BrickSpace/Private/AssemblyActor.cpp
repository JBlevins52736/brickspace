// Fill out your copyright notice in the Description page of Project Settings.


#include "AssemblyActor.h"
#include "Assembly.h"
//#include <Kismet/GameplayStatics.h>

// Sets default values
AAssemblyActor::AAssemblyActor() : assembly(nullptr)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

// Called when the game starts or when spawned
void AAssemblyActor::BeginPlay()
{
	Super::BeginPlay();

	ENetMode CurrentNetMode = GetNetMode();
	if (CurrentNetMode == NM_ListenServer)
	{
		// Create and initialize the Scene Component
		assembly = FindComponentByClass<UAssembly>();

		// Set it as the root component of the Actor
		SetRootComponent(assembly);
	}
}

//void AAssemblyActor::Server_TryAdvanceLayer_Implementation()
//{
//	if (assembly) 
//		assembly->TryAdvanceLayer();
//}

void AAssemblyActor::Server_Move_Implementation(const FTransform& transform)
{
	SetActorTransform(transform);
}


// Called every frame
void AAssemblyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

