// Fill out your copyright notice in the Description page of Project Settings.


#include "ColocationSetup.h"
#include <IPAddress.h>
#include <SocketSubsystem.h>

FString ColocationSetup::GetLocalIPAsString()
{
	bool canBind = false;
	TSharedRef<FInternetAddr> localIp = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLocalHostAddr(*GLog, canBind);
	return (localIp->IsValid() ? localIp->ToString(false) : "0.0.0.0");
}

TArray<uint8> ColocationSetup::CreateByteArrayFromString(const FString& StringToConvert)
{
	TArray<uint8> arr;
	arr.SetNumUninitialized(StringToConvert.Len());
	StringToBytes(StringToConvert, arr.GetData(), StringToConvert.Len());

	return arr;
}

bool ColocationSetup::JoinColocationSession(const FOculusXRColocationSession& Session)
{
	FString IP = BytesToString(Session.Metadata.GetData(), Session.Metadata.Num());
	GEngine->SetClientTravel(GWorld, *IP, ETravelType::TRAVEL_Absolute);

	return true;
}

