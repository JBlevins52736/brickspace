// Fill out your copyright notice in the Description page of Project Settings.


#include "Brick.h"

void UBrick::BeginPlay()
{
	Super::BeginPlay();
	UStaticMeshComponent* mesh = Cast<UStaticMeshComponent>(clientComponent);
	if (mesh != nullptr)
	{
		mesh->OnComponentBeginOverlap.AddDynamic(this, &UBrick::OnOverlapBegin);
		mesh->OnComponentEndOverlap.AddDynamic(this, &UBrick::OnOverlapEnd);
	}

	FVector stud = FVector::ZeroVector;

	studs.push_back(stud);
	FVector tube = stud;
	tube.Z = stud.Z - 32.0;
	tubes.push_back(tube);
}

void UBrick::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// OverlappedComp is our local clientComponent
	// Search OtherActor for Brick components
	for (int i = 0; i < OtherComp->GetNumChildrenComponents(); i++) {
		UBrick* enteringBrick = Cast<UBrick>(OtherComp->GetChildComponent(i));
		if (enteringBrick != nullptr)
		{
			othBrick = enteringBrick;
			UE_LOG(LogTemp, Warning, TEXT("OnOverlapBegin OthBrick:%s"), *FString(othBrick->GetName()));
		}
	}
}

void UBrick::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UVodget* exitingBrick = nullptr;
	for (int i = 0; i < OtherComp->GetNumChildrenComponents(); i++) {
		exitingBrick = Cast<UVodget>(OtherComp->GetChildComponent(i));

		// Only consider volumes with vodgets
		if (exitingBrick != nullptr && othBrick != nullptr) {
			UE_LOG(LogTemp, Warning, TEXT("OnOverlapEnd OthBrick:%s"), *FString(othBrick->GetName()));
			othBrick = nullptr;
		}
	}
}

FVector UBrick::GetWorldStud(int studind)
{
	return clientComponent->GetComponentTransform().TransformPosition(studs[studind]);
}

void UBrick::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (othBrick != nullptr) {

		if (snaptubeind != -1) {
			//if (bestdist <= 80)
			//{
			//	FVector worldtube = clientComponent->GetComponentTransform().TransformPosition(tubes[snaptubeind]);
			//	FVector worldstud = othBrick->GetWorldStud(snapstudind);
			//	FVector ds = worldstud - worldtube;
			//	clientComponent->AddWorldOffset(ds);
			//}
		}
		else {

			float bestdist = FLT_MAX;

			// Snapping down: Compare our tubes with othBrick's studs.
			for (int tubeind = 0; tubeind < tubes.size(); ++tubeind) {
				for (int studind = 0; studind < othBrick->studs.size(); ++studind) {
					FVector othstud = othBrick->GetWorldStud(studind);
					float dist = (tubes[tubeind] - othstud).Length();
					if (dist < bestdist)
					{
						bestdist = dist;
						snaptubeind = tubeind;
						snapstudind = studind;
					}
				}
			}

			//if (bestdist <= 80) 
			{
				FVector worldtube = clientComponent->GetComponentTransform().TransformPosition(tubes[snaptubeind]);
				FVector worldstud = othBrick->GetWorldStud(snapstudind);
				FVector ds = worldstud - worldtube;
				clientComponent->AddWorldOffset(ds);
			}
		}

		// Snapping up (future): Compare our studs with othBricks's tubes.

	}
}