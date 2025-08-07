#include "SmartAnchor.h"
#include "GameFramework/Actor.h"

USmartAnchor::USmartAnchor()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USmartAnchor::BeginPlay()
{
	Super::BeginPlay();
}

void USmartAnchor::CreateAnchor(const bool Value)
{
	Marker->SetWorldLocation(controller->GetComponentLocation());


}

void USmartAnchor::OnCreateAnchorComplete(EOculusXRAnchorResult::Type Result, UOculusXRAnchorComponent* Anchor)
{
	
}
