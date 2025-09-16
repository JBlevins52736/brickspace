// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldAlignmentActor.h"
#include "BrickEreaser.h"
#include "Kismet/GameplayStatics.h"
#include "AlignWorld.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AWorldAlignmentActor::AWorldAlignmentActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AWorldAlignmentActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWorldAlignmentActor::OnRep_ChangeTransform()
{
	endTransform = endTransferTransform;
	startTransform = beginTransferTransform;
	//SetAnchorPostions(startTransform); // this should get overloaded for this case using start makes it so its fine and executes
}

// Called every frame
void AWorldAlignmentActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWorldAlignmentActor::SetAnchorPostions(FTransform anchorPostion)
{
	if (endTransform.Equals(FTransform::Identity))
	{
		;
		endTransform = anchorPostion;
	}
	else
	{
		FTransform offsetTransform = anchorPostion * GetParentActor()->GetTransform().Inverse();
		startTransform = offsetTransform;;
	}

	if (!endTransform.Equals(FTransform::Identity) && !startTransform.Equals(FTransform::Identity) && HasAuthority())
	{
		
		FVector dirBetweenAnchors = startTransform.GetLocation()-endTransform.GetLocation();
		FRotator rotBetweenAnchors = dirBetweenAnchors.Rotation();
		beginTransferTransform = startTransform;
		endTransferTransform = endTransform;
		TArray<AActor*> worldActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), worldActors);
		USceneComponent* directionalLightTargetComp = nullptr;
		for (AActor* actor : worldActors)
		{
			if (actor == this) continue;
			if (actor->FindComponentByClass<UBrickEreaser>() != nullptr) // wall
			{
				actor->SetActorLocation(endTransform.GetLocation());
				actor->SetActorRotation(rotBetweenAnchors);
				USceneComponent* rootComp = actor->GetRootComponent();
				if (rootComp)
				{
					rootComp->SetMobility(EComponentMobility::Static);
					
				}
				
			}
			else if (actor->FindComponentByClass<UAlignWorld>() != nullptr) // ground plate
			{
				actor->SetActorLocation(startTransform.GetLocation());
				actor->SetActorRotation(rotBetweenAnchors);
				USceneComponent* rootComp = actor->GetRootComponent();
				if (rootComp)
				{
					rootComp->SetMobility(EComponentMobility::Static);
				}
			}
			else if (actor->GetName().StartsWith("DirectionalLight"))
			{
				actor->GetRootComponent()->SetMobility(EComponentMobility::Movable);
				FVector direction = endTransform.GetLocation() - actor->GetActorLocation();
				FRotator rotDirection = direction.Rotation();
				actor->SetActorRotation(rotDirection);
				
			}
		}
		
	}
	else
	{
		// clients need to synchronize to the server
	}
}

void AWorldAlignmentActor::Server_UpdateClientTransforms_Implementation(AActor* target,FTransform finalTransform, FTransform beginTransform)
{

	AWorldAlignmentActor* thisActor  = Cast<AWorldAlignmentActor>(target);
	if (thisActor)
	{
		thisActor->endTransferTransform = finalTransform;
		thisActor->beginTransferTransform = beginTransform;
		thisActor->OnRep_ChangeTransform();
	}
}

void AWorldAlignmentActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWorldAlignmentActor, endTransferTransform);
	DOREPLIFETIME(AWorldAlignmentActor, beginTransferTransform);
}


