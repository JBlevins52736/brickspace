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
			switch (brickState) {
			case BrickState::Seeking:
				break;
			case BrickState::Snapping:
				break;
			case BrickState::PinSnapped:
				break;
			case BrickState::RigidSnapped:
				break;
			};
			brickState = BrickState::Snapping;

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

			switch (brickState) {
			case BrickState::Seeking:
				break;
			case BrickState::Snapping:
				break;
			case BrickState::PinSnapped:
				break;
			case BrickState::RigidSnapped:
				break;
			};

			UE_LOG(LogTemp, Warning, TEXT("OnOverlapEnd OthBrick:%s"), *FString(othBrick->GetName()));
			othBrick = nullptr;
		}
	}
}

FVector UBrick::GetWorldStud(int studind)
{
	return clientComponent->GetComponentTransform().TransformPosition(studs[studind]);
}

void UBrick::UpdateSnaps()
{
	// Compare our tubes with othBrick's studs.
	float bestdist = FLT_MAX;
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
	//{
	//	FVector worldtube = clientComponent->GetComponentTransform().TransformPosition(tubes[snaptubeind]);
	//	FVector worldstud = othBrick->GetWorldStud(snapstudind);
	//	FVector ds = worldstud - worldtube;
	//	clientComponent->AddWorldOffset(ds);
	//}
}


void UBrick::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	// States: 
	// Seeking: No overlap with other bricks, normal grabbing mode only.
	// Snapping: Normal grabbing mode followed by tube/stud proximity matches.
	// PinSnapped: Unsnap stress test. If fail: Enter Snapping/Seeking mode. Otherwise: Pin rotate followed by tube/stud proximity matches.
	// RigidSnapped: Unsnap stress test only.

	if (brickState == Seeking) {
		Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
		return;
	}

	if (brickState == Snapping) {
		Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

		UpdateSnaps();


		return;
	}

	FTransform snappedTransform = clientComponent->GetComponentTransform();
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//switch (brickState) {
	//case BrickState::Seeking:
	//case BrickState::Snapping:
	//	break;
	//case BrickState::PinSnapped:
	//case BrickState::RigidSnapped:
	//	break;
	//};

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