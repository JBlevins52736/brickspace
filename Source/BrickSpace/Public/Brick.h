// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Grabber.h"
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
	enum BrickState { Seeking, Snapping, PinSnapped, RigidSnapped };

	FVector GetWorldStud(int studind);

	std::vector<FVector> studs; // 80cm spacing.
	std::vector<FVector> tubes; // -32cm (Z) from studs.

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

	void UpdateSnaps();

	BrickState brickState = BrickState::Seeking;

	class BrickSnap {
	public:
		UBrick* othBrick = nullptr;
		// For snapping tube over other stud.
		int tubeind = -1;
		int othstudind = -1;
		// For snapping stud under other tube.
		int studind = -1;
		int othtubeind = -1;
	};

	std::list<BrickSnap> snaps;

	UBrick* othBrick = nullptr;
	int snaptubeind = -1;
	int snapstudind = -1;
};
