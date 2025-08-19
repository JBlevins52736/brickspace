// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Vodget.h"
#include "Components/TextRenderComponent.h"

#include "MenuItem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMenuItemSelectedEvent); // Declares a delegate type named FMyNoArgsEvent

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BRICKSPACE_API UMenuItem : public UVodget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable, Category = "VAR")
	FMenuItemSelectedEvent onMenuItemSelected;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (Bitmask), Category = "VAR")
	UTextRenderComponent *textRender;
	
	

	

	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void Focus(USelector* selector, bool state) override;
	virtual void ForePinch(USelector* selector, bool state) override;


	
	


protected:

	FText menuItemText;

	

	FLinearColor colorHover;
	FLinearColor colorNormal;
	FLinearColor colorSelect;



	
	UMaterialInstanceDynamic* DynamicMaterialInstance;
};
