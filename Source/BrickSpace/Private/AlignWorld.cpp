// Fill out your copyright notice in the Description page of Project Settings.


#include "AlignWorld.h"

#include "WorldAlignmentActor.h"

void UAlignWorld::ForePinch(USelector* selector, bool state)
{
	Super::ForePinch(selector, state);

	if (!state)
	{
		FTransform transform = clientComponent->GetComponentTransform();
		AActor* actor = GetOwner();
		AWorldAlignmentActor* worldAlignmentActor = Cast<AWorldAlignmentActor>(actor);
		if (worldAlignmentActor)
		{
			GEngine->AddOnScreenDebugMessage(-1,1, FColor::Red, TEXT("This is working"));
			worldAlignmentActor->SetAnchorPostions((transform));
		}
	}
	
}
