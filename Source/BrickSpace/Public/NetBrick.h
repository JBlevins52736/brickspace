#pragma once

#include "CoreMinimal.h"
#include "Brick.h"
#include "Net/UnrealNetwork.h"
#include "NetBrick.generated.h"

/**
 * NetBrick - Colocation-synced Brick for shared VR space
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BRICKSPACE_API UNetBrick : public UBrick
{
	GENERATED_BODY()

public:
	UNetBrick();

	virtual void ForePinch(USelector* selector, bool state) override;
	
protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_UpdateTransform(const FVector& Location, const FRotator& Rotation);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetGrabbing(bool bGrabbed);

private:
	UPROPERTY(Replicated)
	bool bIsGrabbed = false;
};