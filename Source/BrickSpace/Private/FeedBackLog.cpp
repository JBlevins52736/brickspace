// Fill out your copyright notice in the Description page of Project Settings.


#include "FeedBackLog.h"

void UFeedBackLog::BeginPlay()
{
	Super::BeginPlay();

	///feedbackUI->SetVisibility(false);
	colorAsync = FLinearColor(0.833333f, 0.764656f, 0.117188f, 1.0f); // Red, Green, Blue, Alpha
	colorAsyncSuccess = FLinearColor(0.046658f, 0.1875f, 0.05532f, 1.0f);
	colorAsyncFailure = FLinearColor(0.1875f, 0.053711f, 0.076189f, 1.0f);

	//DynamicMaterialInstance = UMaterialInstanceDynamic::Create(icon->GetMaterial(0), this);
}


// only testing this
void UFeedBackLog::EnterAsync()
{

	// when enter here turn off the menu
	getMesh->SetVisibility(false);
	
	

	/*DynamicMaterialInstance->SetVectorParameterValue(TEXT("Color"), colorAsync);*/
	//textRender->SetText(FText::FromString("PLEASE WAIT"));
	//LogMessage("Async operation");
}




void UFeedBackLog::ExitAsync(bool Success)
{
	

	if (Success) {
		DynamicMaterialInstance->SetVectorParameterValue(TEXT("Color"), colorAsyncSuccess);
		LogMessage("Success");
	}
	else {
		DynamicMaterialInstance->SetVectorParameterValue(TEXT("Color"), colorAsyncFailure);
		LogMessage("Failure");
	}

	
}


void UFeedBackLog::LogMessage(FString errorMsg)
{
	if (log == nullptr)
		return;

	log->SetText(FText::FromString(errorMsg));
	log->SetVisibility(true);
	PrimaryComponentTick.SetTickFunctionEnable(true);
	logVisiblitySecsRemaining = 3.0;
}


void UFeedBackLog::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	logVisiblitySecsRemaining -= DeltaTime;
	if (logVisiblitySecsRemaining < 0.0) {
		log->SetVisibility(false);
		PrimaryComponentTick.SetTickFunctionEnable(false);
	}
}