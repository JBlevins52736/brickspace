#include "NetBrick.h"

UNetBrick::UNetBrick()
{
	SetIsReplicatedByDefault(true);
}

void UNetBrick::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UNetBrick, bIsGrabbed);
}

void UNetBrick::ForePinch(USelector* selector, bool state)
{
	// Sync grab state
	

	Super::ForePinch(selector, state);

	bIsGrabbed = state;
	Multicast_SetGrabbing(state);
}

void UNetBrick::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// If I'm grabbing, send my position
	if ( bIsGrabbed)
	{
		FVector Loc = clientComponent->GetComponentLocation();
		FRotator Rot = clientComponent->GetComponentRotation();
		Multicast_UpdateTransform(Loc, Rot);
	}
}

void UNetBrick::Multicast_SetGrabbing_Implementation(bool bGrabbed)
{
	bIsGrabbed = bGrabbed;
}

void UNetBrick::Multicast_UpdateTransform_Implementation(const FVector& Location, const FRotator& Rotation)
{
	// Only apply if I'm not the one grabbing

		clientComponent->SetWorldLocationAndRotation(Location, Rotation);
	
}