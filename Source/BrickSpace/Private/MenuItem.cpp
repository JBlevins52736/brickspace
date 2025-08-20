// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuItem.h"
#include "Selector.h"

void UMenuItem::BeginPlay()
{
	Super::BeginPlay();

	

	// Typical icon feedback colors
	colorHover = FLinearColor(0.047059f, 0.070588f, 0.101961f, 1.0f);
	colorNormal = FLinearColor(0.019608f, 0.023529f, 0.027451f, 1.0f);
	colorSelect = FLinearColor(0.062745f, 0.094118f, 0.137255f, 1.0f);

	// Replace static icon material with a dynamic material instance.
	UStaticMeshComponent *icon = Cast<UStaticMeshComponent>(clientComponent);
	DynamicMaterialInstance = UMaterialInstanceDynamic::Create(icon->GetMaterial(0), this);
	icon->SetMaterial(0, DynamicMaterialInstance);

	DynamicMaterialInstance->SetVectorParameterValue(TEXT("Color"), colorNormal);
	menuItemText = textRender->Text;

	
}

void UMenuItem::Focus(USelector* selector, bool state)
{
	

	if (state)
		DynamicMaterialInstance->SetVectorParameterValue(TEXT("Color"), colorHover);
	else
		DynamicMaterialInstance->SetVectorParameterValue(TEXT("Color"), colorNormal);

	textRender->SetText(menuItemText);
}

void UMenuItem::ForePinch(USelector* selector, bool state)
{
	

	selector->GrabFocus(state);
	if (state) {
		DynamicMaterialInstance->SetVectorParameterValue(TEXT("Color"), colorSelect);
		onMenuItemSelected.Broadcast();
	}
	else {
		DynamicMaterialInstance->SetVectorParameterValue(TEXT("Color"), colorHover);
	}
	textRender->SetText(menuItemText);
}






