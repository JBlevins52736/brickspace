// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldAlignmentActor.h"
#include "BrickEreaser.h"
#include "Kismet/GameplayStatics.h"
#include "AlignWorld.h"
#include "Net/UnrealNetwork.h"
#include "BrickSpacePawn.h"

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
		
		startTransform = anchorPostion;
	}

	if (!endTransform.Equals(FTransform::Identity) && !startTransform.Equals(FTransform::Identity) && HasAuthority())
	{
		
		FVector fwdAnchorRefDir = endTransform.GetLocation() - startTransform.GetLocation();
		AController* controller = GetWorld()->GetFirstPlayerController();
		APlayerController* playerCont = Cast<APlayerController>(controller);
		if (playerCont) {
			AActor* actor = playerCont->GetPawn();
			ABrickSpacePawn* serverPawn = Cast<ABrickSpacePawn>(actor);
			serverPawn->SetActorLocation(startTransform.GetLocation());
			TArray<AActor*> actors;
			UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("manufacturer"), actors);
			if (actors.Num() > 0) {
				FVector dirToWall = actors[0]->GetActorLocation() - serverPawn->GetActorLocation();
				dirToWall.Z = 0;
				fwdAnchorRefDir.Z = 0;
				FQuat rotToWall = FQuat::FindBetweenVectors(dirToWall, fwdAnchorRefDir);
				serverPawn->SetActorRotation(rotToWall);
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


