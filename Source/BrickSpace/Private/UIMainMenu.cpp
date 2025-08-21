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
	FTransform newloc = loc->GetComponentTransform();

	

}
