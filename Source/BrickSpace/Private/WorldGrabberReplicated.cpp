// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldGrabberReplicated.h"
#include "BrickSpacePawn.h"
#include "BrickSpaceGameState.h"
#include <Kismet/GameplayStatics.h>
#include "Json.h"
#include "JsonUtilities.h"

void UWorldGrabberReplicated::BeginPlay()
{
	Super::BeginPlay();
	if (GetOwner()->HasAuthority())
		scaleMode = true;

	LoadCalibration();
}

void UWorldGrabberReplicated::ActiveChanged()
{
	Super::ActiveChanged();
	if (!activeMode)
		SaveCalibration();
}

void UWorldGrabberReplicated::OnRep_WorldScale(float worldScale)
{
	//if (currWorldToMeters == worldScale)
	//	return;

	// The BrickSpaceGameState currWorldToMeters property was just changed by the listening server client.
	// Update our local copy of currWorldToMeters and scale the local and replicated hand meshes.
	SetWorldToMeters(worldScale);

	// Scale replicated hands via server so all clients are updated.
	if (leftHandReplicated && rightHandReplicated)
	{
		float handScale = currWorldToMeters / 100.0;
		FVector scaleVec = FVector::OneVector * handScale;

		ABrickSpacePawn* brickSpacePawn = Cast<ABrickSpacePawn>(GetOwner());
		if (brickSpacePawn->HasAuthority()) {
			// Scale replicated hands immediately when listen server client is updated. 
			leftHandReplicated->SetWorldScale3D(scaleVec);
			rightHandReplicated->SetWorldScale3D(scaleVec);
		}
		else {
			// Have server update replicated mesh scales.
			brickSpacePawn->Server_MeshScaleUpdate(leftHandReplicated, rightHandReplicated, handScale);
		}
	}
}


void UWorldGrabberReplicated::SetWorldToMeters(float worldScale)
{
	// Only the host can change replicated world scale.
	if (GetOwner()->HasAuthority()) {
		if (currWorldToMeters == worldScale)
			return;
		Super::SetWorldToMeters(worldScale);

		// Notify the GameState so all clients can be updated.
		ABrickSpaceGameState* GameState = Cast<ABrickSpaceGameState>(UGameplayStatics::GetGameState(GetWorld()));
		if (GameState)
		{
			GameState->currWorldToMeters = worldScale;
			GameState->OnRep_WorldScale();
		}
	}
}

void UWorldGrabberReplicated::LoadCalibration()
{
	FString file = FPaths::ProjectConfigDir();
	file.Append(TEXT("Calibration.json"));

	// We will use this FileManager to deal with the file.
	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();

	FString JsonString;

	// Always first check if the file that you want to manipulate exist.
	if (FileManager.FileExists(*file))
	{
		// We use the LoadFileToString to load the file into
		if (FFileHelper::LoadFileToString(JsonString, *file, FFileHelper::EHashOptions::None))
		{
			UE_LOG(LogTemp, Warning, TEXT("FileManipulation: Text From File: %s"), *JsonString);

			// Create a TSharedPtr to hold the FJsonObject
			TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());

			// Create a TJsonReader to read the JsonString
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

			// Attempt to deserialize the JSON string into the FJsonObject
			if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
			{
				// Successfully deserialized the JSON string into JsonObject
				FCalibration calibration;
				if (FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &calibration, 0, 0))
				{
					SetWorldLocation(calibration.position);
					SetWorldRotation(calibration.rotation);
					SetWorldToMeters(calibration.worldToMeters);

					UE_LOG(LogTemp, Error, TEXT("Converted Local FJsonObject to FCalibration."));
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Failed to convert FJsonObject to FCalibration."));
				}
			}
			else
			{
				// Error deserializing the JSON string
				UE_LOG(LogTemp, Error, TEXT("Failed to deserialize JSON string."));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("FileManipulation: Did not load text from file"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("FileManipulation: ERROR: Can not read the file because it was not found."));
		UE_LOG(LogTemp, Warning, TEXT("FileManipulation: Expected file location: %s"), *file);
	}
}

void UWorldGrabberReplicated::SaveCalibration()
{
	// Save calibration to json file.
	FCalibration calibration;
	calibration.position = GetComponentLocation();
	calibration.rotation = GetComponentQuat();
	calibration.worldToMeters = GetWorld()->GetWorldSettings()->WorldToMeters;

	TSharedPtr<FJsonObject> JsonObject = FJsonObjectConverter::UStructToJsonObject(calibration);
	if (JsonObject != nullptr)
	{
		FString JsonString;
		if (!FJsonSerializer::Serialize(JsonObject.ToSharedRef(), TJsonWriterFactory<>::Create(&JsonString, 0)))
		{
			UE_LOG(LogTemp, Warning, TEXT("FJsonSerializer: ERROR: Failed to convert JsonObject to string."));
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("FJsonSerializer: SUCCESS\n%s"), *JsonString);

			FString file = FPaths::ProjectConfigDir();
			file.Append(TEXT("Calibration.json"));

			// We will use this FileManager to deal with the file.
			IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();

			if (FFileHelper::SaveStringToFile(JsonString, *file))
			{
				UE_LOG(LogTemp, Warning, TEXT("FileManipulation: Succesfuly written: \"%s\" to the text file"), *JsonString);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("FileManipulation: Failed to write FString to file."));
			}
		}
	}
}
