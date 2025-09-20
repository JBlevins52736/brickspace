#pragma once

#include "CoreMinimal.h"
#include "Grabber.h"
#include "Stud.h"
#include "Tube.h"
#include <list>
#include <vector>

class AAssemblyActor;
class ABrickSpacePlayerState;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VAR")
	FString shortName;

	UPROPERTY(EditAnywhere, Category = "VAR")
	bool poseIsUnique = true;

	UPROPERTY(EditAnywhere, Replicated)
	bool isSolid = true;

	UPROPERTY(ReplicatedUsing = OnRep_Grabbable)
	bool isGrabbable = true;

	UFUNCTION()
	virtual void OnRep_Grabbable();

	UFUNCTION(Server, Reliable)
	void UpdateMaterialOnGrab(UMaterialInterface* color, USelector* selector);

	UFUNCTION(Server, Reliable)
	void Server_SetMaterial(UMaterialInterface* color);

	UPROPERTY(ReplicatedUsing = OnRep_Material)
	UMaterialInterface* brickMaterial;

	UFUNCTION()
	virtual void OnRep_Material();

	UPROPERTY(Replicated)
	UMaterialInterface* solidMatchMaterial = nullptr;	// Set by Reveal() method and only used to test material in TryMatch() 

	UPROPERTY(ReplicatedUsing = OnRep_Parent)
	AAssemblyActor* assemblyActor = nullptr;

	UFUNCTION()
	virtual void OnRep_Parent();

	FVector GetLocation();
	FQuat GetQuat();
	UMaterialInterface* GetMaterial();

	bool TryReparent(USceneComponent* pnt, std::vector<UBrick*>& layerBricks);
	void ReparentConnectedBricks(USceneComponent *pnt, std::vector<UBrick*> &layerBricks);

	//void Reveal(UMaterialInterface* revealMaterial, UMaterialInterface* brickMaterial);
	bool TryMatch( UBrick *assemblerBrick );
	bool IsSolved() { return isSolid; }

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void GetAndSetMatColorFromPlayer(USelector* selector);

	ABrickSpacePlayerState* playerState = nullptr;

private:
	/** called when something enters the sphere component */
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/** called when something leaves the sphere component */
	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void TryBreakSnaps();
	void FindSnaps();
	void SolveSnaps();

	void DoExplodeMismatchedEffect();
	void DoAcceptMatchWithRevealedBrick();

	std::list<UBrick *> overlappedBricks;	// Note: Assumes no bricks are actually deleted.

};
