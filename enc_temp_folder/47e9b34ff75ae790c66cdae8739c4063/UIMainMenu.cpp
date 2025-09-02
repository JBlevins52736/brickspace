// Fill out your copyright notice in the Description page of Project Settings.


#include "UIMainMenu.h"
#include "Kismet/GameplayStatics.h"

void UUIMainMenu::changeScene(bool F)
{
	if (F == true)
	{
		UGameplayStatics::OpenLevel(GetWorld(), FName("BrickSpace"));
	}
}

void UUIMainMenu::SpawnMenu()
{




}
void UUIMainMenu::UpdateUIScreenComp()
{
	

	if (!camera) return;

	FVector cameraPos = camera->GetComponentTransform().GetLocation();

	FVector UIPos = GetOwner()->GetActorLocation();

	FVector direction = UIPos - cameraPos;

	direction.Normalize();

	float dot = FVector::DotProduct(camera->GetForwardVector(), direction);
	if (dot < 0.10f) // move the UI to a location, lerp over time
	{
		//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Orange, FString::Printf(TEXT("Dot: %f"), dot));
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, FString::Printf(TEXT("x: %f, y: %f, z: %f"), UIPos.X, UIPos.Y, UIPos.Z));
		FVector updatePosition = cameraPos + camera->GetForwardVector() * distanceFromFace;
		updatePosition.Z += 50;
		GetOwner()->SetActorLocation(FMath::Lerp(cameraPos, updatePosition, GetWorld()->GetDeltaSeconds() * 0.5f));

	}
	else return;
}

void UUIMainMenu::BeginPlay()
{
	Super::BeginPlay();
	camera = GetWorld()->GetFirstPlayerController()->GetPawn()->FindComponentByClass<UCameraComponent>();
	FVector startPos = camera->GetComponentTransform().GetLocation() + camera->GetForwardVector() * distanceFromFace;
	startPos.Z += 50;
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Orange, FString::Printf(TEXT("Begin play x: %f, y: %f, z: %f"), startPos.X, startPos.Y, startPos.Z));
	this->GetOwner()->SetActorLocation(startPos);
	FQuat rot = FQuat(1, 1, 180, 1);
	GetOwner()->SetActorRotation(rot);
}


