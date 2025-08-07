#include "SmartAnchor.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Components/StaticMeshComponent.h"

USmartAnchor::USmartAnchor()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USmartAnchor::BeginPlay()
{
	Super::BeginPlay();

	//LoadAnchors();
}

void USmartAnchor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (isAddAnchor)
	{
		Marker->AddLocalOffset(dir * DeltaTime * 25);
		if (dir.Z > 0.0 && Marker->GetComponentLocation().Z > toVec.Z)
		{
			dir = FVector::DownVector;
		}
		else if (dir.Z < 0.0 && Marker->GetComponentLocation().Z < fromVec.Z)
		{
			dir = FVector::UpVector;
		}
	}
}



void USmartAnchor::CreateAnchor(const bool Value)
{
	FTransform ControllerTransform = Controller->GetComponentTransform();

	UWorld* World = GetWorld();


	USceneComponent* AnchorSceneComponent = NewObject<USceneComponent>(this, USceneComponent::StaticClass(), TEXT("AnchorRoot"));
	AnchorSceneComponent->SetWorldTransform(ControllerTransform);
	AnchorSceneComponent->RegisterComponent();

	UStaticMeshComponent* MeshComponent = NewObject<UStaticMeshComponent>(AnchorSceneComponent, UStaticMeshComponent::StaticClass(), TEXT("AnchorMesh"));
	
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComponent->SetNotifyRigidBodyCollision(false);
	if (AnchorMesh)
	{
		MeshComponent->SetStaticMesh(AnchorMesh);
	}
	else
	{
		UStaticMesh* SphereMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Sphere.Sphere"));
		if (SphereMesh)
		{
			MeshComponent->SetStaticMesh(SphereMesh);
			MeshComponent->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));
		}
	}

	MeshComponent->AttachToComponent(AnchorSceneComponent, FAttachmentTransformRules::KeepRelativeTransform);
	MeshComponent->RegisterComponent();

	// Create spatial anchor attached to the scene component's owner
	EOculusXRAnchorResult::Type OutResult;
	bool bSuccess = OculusXRAnchors::FOculusXRAnchors::CreateSpatialAnchor(
		ControllerTransform,
		GetOwner(),
		FOculusXRSpatialAnchorCreateDelegate::CreateLambda([this](EOculusXRAnchorResult::Type Result, UOculusXRAnchorComponent* Anchor)
			{
				if (Result == EOculusXRAnchorResult::Success && IsValid(Anchor))
				{
					SavedAnchors.Add(Anchor);
				}
			}),
		OutResult
	);



	Marker->SetWorldLocation(Controller->GetComponentLocation());

	/*FTransform GetActor = anch->GetActorTransform();
	FTransform MyActorTransform = GetOwner()->GetActorTransform();*/
	isAddAnchor = true;
	fromVec = Marker->GetComponentLocation();
	toVec = fromVec + (FVector::UpVector * 50.0f);
	dir = FVector::UpVector;

	Marker->AttachToComponent(AnchorSceneComponent,FAttachmentTransformRules::KeepRelativeTransform);
	Marker->SetRelativeLocation(FVector(0, 0, 0));

}

void USmartAnchor::LoadAnchors()
{
	// Simple anchor loading - will implement properly once creation works
}

void USmartAnchor::ClearAllAnchors()
{
	SavedAnchors.Empty();
}