#include "Selector.h"
#include "Vodget.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
USelector::USelector() : handMaterial(nullptr)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

void USelector::ForePinch(const bool Value)
{
	if (focusVodget != nullptr)
		focusVodget->ForePinch(this, Value);
}
void USelector::MiddlePinch(const bool Value)
{
	if (focusVodget != nullptr)
		focusVodget->MiddlePinch(this, Value);
}
void USelector::Grip(const bool Value)
{
	if (focusVodget != nullptr)
		focusVodget->Grip(this, Value);
}

// Inheriting selectors must all maintain a model of a 3D cursor in world coordinates. 
const FTransform& USelector::Cursor()
{
	// Inheriting selectors must all override SetCursor() to set FTransform cursor based on the selection method. 
	SetCursor();
	return cursor;
}

void USelector::SetFilter(uint16 filter)
{
	selectionFilter = filter;
}


void USelector::VARLog(FString methodName)
{
	APawn* pawn = Cast<APawn>(GetOwner());
	if (!pawn) {
		UE_LOG(LogTemp, Error, TEXT("Log() could not cast owner to pawn in HandSelector.cpp"));
		return;
	}

	FString locstr = (pawn->IsLocallyControlled()) ? TEXT("LocallyControlled") : TEXT("NotLocallyControlled");
	switch (pawn->GetLocalRole())
	{
	case ROLE_Authority:		// Server Actor version: This is also the hosting listen client version.
		UE_LOG(LogTemp, Warning, TEXT("%s: ROLE_Authority: %s"), *methodName, *locstr);
		break;
	case ROLE_AutonomousProxy:	// Non-Server Player Actor version: IsLocallyControlled should be true
		UE_LOG(LogTemp, Warning, TEXT("%s: ROLE_AutonomousProxy: %s"), *methodName, *locstr);
		break;
	case ROLE_SimulatedProxy:	// Non-Server Ghost
		UE_LOG(LogTemp, Warning, TEXT("%s: ROLE_SimulatedProxy: %s"), *methodName, *locstr);
		break;
	}
}

void USelector::SetMaterial(UMaterialInterface* color)
{
	VARLog(TEXT("USelector::SetMaterial"));

	APawn* pawn = Cast<APawn>(GetOwner());
	if (!pawn) {
		UE_LOG(LogTemp, Error, TEXT("Log() could not cast owner to pawn in HandSelector.cpp"));
		return;
	}

	if (pawn->HasAuthority()) {
		UE_LOG(LogTemp, Warning, TEXT("SetMaterial called from server client"));
		Server_SetMaterial_Implementation(color);
	}
	else if (pawn->GetLocalRole() == ROLE_AutonomousProxy) {
		UE_LOG(LogTemp, Warning, TEXT("SetMaterial called from remote client"));
		Server_SetMaterial(color);
	}
}

void USelector::Server_SetMaterial_Implementation(UMaterialInterface* color)
{
	VARLog(TEXT("USelector::Server_SetMaterial_Implementation"));

	handMaterial = color;
	OnRep_Material();
}

void USelector::OnRep_Material()
{
	VARLog(TEXT("USelector::OnRep_Material"));

	if (handMaterial)
		handMesh->SetMaterial(0, handMaterial);
}

void USelector::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USelector, handMaterial);

}

// This method should be pure virtual but is implemented to avoid Unreal compiler issues.
void USelector::SetCursor() {}
