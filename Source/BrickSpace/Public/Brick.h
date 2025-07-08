#pragma once

#include "CoreMinimal.h"
#include "Grabber.h"
#include "Stud.h"
#include "Tube.h"
#include <list>

#include "Brick.generated.h"

/**
 *
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BRICKSPACE_API UBrick : public UGrabber
{
	GENERATED_BODY()

public:

	virtual void ForePinch(USelector* selector, bool state) override;

	std::vector<USnapBase*> studs; // 78cm spacing.
	std::vector<USnapBase*> tubes; // Thin spacing -32cm (Z) from studs.

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	/** called when something enters the sphere component */
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** called when something leaves the sphere component */
	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	std::list<UBrick *> bricks;	// Note: Assumes no bricks are actually deleted.
};
