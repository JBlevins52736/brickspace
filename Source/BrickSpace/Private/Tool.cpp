// Fill out your copyright notice in the Description page of Project Settings.


#include "Tool.h"
#include <Kismet/GameplayStatics.h>
#include "BrickSpacePlayerState.h"
#include "Selector.h"


void UTool::BeginPlay()
{
	Super::BeginPlay();

	//selectionFilter = 0x01;

	UStaticMeshComponent* mesh = Cast<UStaticMeshComponent>(clientComponent);
	if (mesh != nullptr)
	{
		// Add overlap with bricks.
		mesh->OnComponentBeginOverlap.AddDynamic(this, &UTool::OnOverlapBegin);
	}
}

void UTool::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// OverlappedComp is our local clientComponent
	// Search OtherActor for Brick components
	for (int i = 0; i < OtherComp->GetNumChildrenComponents(); i++) {
		brickTouched = Cast<UBrick>(OtherComp->GetChildComponent(i));
		if (brickTouched != nullptr)
		{
			ChangeBrickTouched();
			UE_LOG(LogTemp, Warning, TEXT("OnOverlapBegin brickTouched:%s"), *FString(brickTouched->ClientName()));
			break;
		}
	}
}

// Must override in inheriting classes.
void UTool::ChangeBrickTouched() {}
