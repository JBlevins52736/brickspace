// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "OculusXRColocationSession.h"


/**
 * 
 */
class BRICKSPACE_API ColocationSetup : public UBlueprintFunctionLibrary
{
	//GENERATED_BODY()
public:
	/* Gets the Local IP Address of the device */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Oculus|ColocationSession", meta = (DisplayName = "Get Local IP As String"))
	static FString GetLocalIPAsString();

	/* Attempts to join a session based on the colocation session's supplied IP address */
	UFUNCTION(BlueprintCallable, Category = "Oculus|ColocationSession")
	static TArray<uint8> CreateByteArrayFromString(const FString& StringToConvert);

	/* Attempts to join a session based on the colocation session's supplied IP address */
	UFUNCTION(BlueprintCallable, Category = "Oculus|ColocationSession")
	static bool JoinColocationSession(const FOculusXRColocationSession& Session);
};
