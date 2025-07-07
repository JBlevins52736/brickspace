#pragma once

//class UStud;
#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "SnapBase.h"

#include "Tube.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BRICKSPACE_API UTube : public USnapBase
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTube();
};
