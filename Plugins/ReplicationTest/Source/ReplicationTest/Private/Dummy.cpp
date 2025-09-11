// Fill out your copyright notice in the Description page of Project Settings.


#include "Dummy.h"
#include "UObject/ConstructorHelpers.h" // Include for ConstructorHelpers

#include "Net/UnrealNetwork.h" // Required for DOREPLIFETIME

// Sets default values
ADummy::ADummy()
{
	SetReplicates(true);

	CubeMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CubeMesh"));
	RootComponent = CubeMeshComponent;

	// Load the default cube static mesh
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	if (CubeMeshAsset.Succeeded())
	{
		CubeMeshComponent->SetStaticMesh(CubeMeshAsset.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialFinder(TEXT("/ReplicationTest/BryansBS/M_Color.M_Color"));
	if (MaterialFinder.Succeeded())
	{
		// Create a dynamic material instance so we can modify it at runtime
		DynamicMaterialInstance = UMaterialInstanceDynamic::Create(MaterialFinder.Object, CubeMeshComponent);
		if (DynamicMaterialInstance)
		{
			CubeMeshComponent->SetMaterial(0, DynamicMaterialInstance);

		}

		colorVec = FLinearColor::Red;
		OnRep_Color();
	}
}

// Called when the game starts or when spawned
void ADummy::BeginPlay()
{
	Super::BeginPlay();

	CubeMeshComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	CubeMeshComponent->SetGenerateOverlapEvents(true);

	//if (HasAuthority())
	{
		CubeMeshComponent->OnComponentBeginOverlap.AddDynamic(this, &ADummy::OnOverlapBegin);
	}
}

void ADummy::Server_ChangeColor_Implementation(AActor* TargetActor)
{
	FLinearColor color(FMath::RandRange(0.0, 1.0), FMath::RandRange(0.0, 1.0), FMath::RandRange(0.0, 1.0));
	colorVec = color;
	OnRep_Color();

	if (HasAuthority()) {
		UE_LOG(LogTemp, Warning, TEXT("SERVER Color:%f %f %f"), colorVec.R, colorVec.G, colorVec.B);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("CLIENT Color:%f %f %f"), colorVec.R, colorVec.G, colorVec.B);
	}

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