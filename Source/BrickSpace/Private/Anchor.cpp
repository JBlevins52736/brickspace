// Fill out your copyright notice in the Description page of Project Settings.


#include "Anchor.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshSocket.h"
void UAnchor::BeginPlay()
{
	Super::BeginPlay();
	pinDock = Cast<USphereComponent>(GetOwner()->FindComponentByClass<USphereComponent>());
	if (pinDock)
	{

		pinDock->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		pinDock->SetGenerateOverlapEvents(true);
		pinDock->SetCollisionObjectType(ECC_WorldDynamic);
		pinDock->SetCollisionResponseToAllChannels(ECR_Overlap);
		pinDock->OnComponentBeginOverlap.AddDynamic(this, &UAnchor::OnOverlapBegin);
	}
}

void UAnchor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
}



void UAnchor::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
		

		if (OtherActor->ActorHasTag("Test"))
		{
			UE_LOG(LogTemp, Warning, TEXT("Overlapped with Anchor!"));
		}
	
}

void UAnchor::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}