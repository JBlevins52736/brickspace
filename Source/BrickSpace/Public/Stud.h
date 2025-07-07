#pragma once

//class UTube;
#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "SnapBase.h"

#include "Stud.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BRICKSPACE_API UStud : public USnapBase
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStud();
};
