// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Vodget.h"
#include "FocusVodget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FButtonPressed, bool, ButtonID);

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BRICKSPACE_API UFocusVodget : public UVodget
{
	GENERATED_BODY()
	

public:
	virtual void Focus(USelector* cursor, bool state) override;
	
	UPROPERTY(BlueprintAssignable)
	FButtonPressed pressed;
};
