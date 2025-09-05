// Fill out your copyright notice in the Description page of Project Settings.


#include "Dummy.h"
#include "UObject/ConstructorHelpers.h" // Include for ConstructorHelpers

#include "Net/UnrealNetwork.h" // Required for DOREPLIFETIME

// Sets default values
ADummy::ADummy()
{
	CubeMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CubeMesh"));
	RootComponent = CubeMeshComponent;

	// Load the default cube static mesh
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	if (CubeMeshAsset.Succeeded())
	{
		CubeMeshComponent->SetStaticMesh(CubeMeshAsset.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialFinder(TEXT("/Game/BryansBS/M_Color"));
	//static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialFinder(TEXT("/Game/Materials/Translucent_Green"));
	if (MaterialFinder.Succeeded())
	{
		// Create a dynamic material instance so we can modify it at runtime
		DynamicMaterialInstance = UMaterialInstanceDynamic::Create(MaterialFinder.Object, CubeMeshComponent);
		if (DynamicMaterialInstance)
		{
			CubeMeshComponent->SetMaterial(0, DynamicMaterialInstance);
		}
	}
}

// Called when the game starts or when spawned
void ADummy::BeginPlay()
{
	Super::BeginPlay();

	CubeMeshComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	CubeMeshComponent->SetGenerateOverlapEvents(true);

	CubeMeshComponent->OnComponentBeginOverlap.AddDynamic(this, &ADummy::OnOverlapBegin);

	colorVec.R = 1.0;
	colorVec.G = 0.0;
	colorVec.B = 0.0;

	OnRep_Color();
}

void ADummy::Server_ChangeColor_Implementation(AActor* TargetActor)
{
	colorVec.R = FMath::RandRange(0.0, 1.0);
	colorVec.G = FMath::RandRange(0.0, 1.0);
	colorVec.B = FMath::RandRange(0.0, 1.0);
	OnRep_Color();
}

void ADummy::OnRep_Color()
{
	DynamicMaterialInstance->SetVectorParameterValue(TEXT("Color"), colorVec);
}


void ADummy::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Server_ChangeColor_Implementation(this);
}

void ADummy::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADummy, colorVec);
}