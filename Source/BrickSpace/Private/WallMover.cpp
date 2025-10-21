#include "WallMover.h"
#include "WallLever.h" 
#include "Kismet/KismetMathLibrary.h"
#include "BrickSpacePawn.h"
#include "Net/UnrealNetwork.h"

UWallMover::UWallMover()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void UWallMover::BeginPlay()
{
	Super::BeginPlay();

	InitialRelativeLocation = GetRelativeLocation();
	LoweredRelativeLocation = InitialRelativeLocation + FVector(0.0f, 0.0f, LoweredZOffset);
	RaisedRelativeLocation = InitialRelativeLocation + FVector(0.0f, 0.0f, RaisedZOffset);


	TargetRelativeLocation = InitialRelativeLocation;
}

void UWallMover::SetMovementTarget(float Percentage)
{
	CurrentPercentage = Percentage;

	if (Percentage < 0.0f)
	{
		TargetRelativeLocation = LoweredRelativeLocation;
	}
	else if (Percentage > 0.0f)
	{
		TargetRelativeLocation = RaisedRelativeLocation;
	}
	else
	{
		TargetRelativeLocation = GetRelativeLocation();
	}
	PrimaryComponentTick.SetTickFunctionEnable(true);
}


void UWallMover::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UWallMover, TargetRelativeLocation);
	DOREPLIFETIME(UWallMover, CurrentPercentage);
}

void UWallMover::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	float PercentageAbs = FMath::Abs(CurrentPercentage);

	const float BaseSpeedFactor = 2.0f;

	FVector NewLocation = UKismetMathLibrary::VInterpTo(
		GetRelativeLocation(),
		TargetRelativeLocation,
		DeltaTime,
		(BaseSpeedFactor * PercentageAbs)
	);
	//SetRelativeLocation(NewLocation);

	/*APlayerController* LocalPlayerController = GetWorld()->GetFirstPlayerController(); 
		if (LocalPlayerController)
		{
			APawn* LocalPawn = LocalPlayerController->GetPawn();
			if (LocalPawn)
			{
				PrimaryComponentTick.SetTickFunctionEnable(true);
				ABrickSpacePawn* brickspacePawn = Cast<ABrickSpacePawn>(LocalPawn);
				
				if (LocalPawn->HasAuthority())
				{
					SetRelativeLocation(NewLocation);
				}
				else 
				{
					brickspacePawn->Server_Translate(this, NewLocation);
				}

			}
		}*/
	APlayerController* LocalPlayerController = GetWorld()->GetFirstPlayerController();
	if (LocalPlayerController)
	{
		APawn* LocalPawn = LocalPlayerController->GetPawn();
		if (LocalPawn)
		{
		
			FString PawnName = LocalPawn->GetName();

			
			UE_LOG(LogTemp, Warning, TEXT("Local Pawn Name: %s"), *PawnName);


				/*GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("Pawn Name: %s"), *PawnName));*/
		

			PrimaryComponentTick.SetTickFunctionEnable(true);
			ABrickSpacePawn* brickspacePawn = Cast<ABrickSpacePawn>(LocalPawn);

			if (LocalPawn->HasAuthority())
			{
				SetRelativeLocation(NewLocation);
			}
			else
			{
				brickspacePawn->Server_Translate(this, NewLocation);
			}

		}
	}

	//GEngine->AddOnScreenDebugMessage(
	//    -1,                      // Key to identify/replace the message (use -1 for new)
	//    5.0f,                    // Duration to display (in seconds)
	//    FColor::Green,          // Color of the text
	//    FString::Printf(TEXT("Wall Z Location: %f"), NewLocation.Z));

	if (FloorPlate)
	{
		float WallZ = GetRelativeLocation().Z;
		if (WallZ <= 10) {

			FVector CurrentScale = FloorPlate->GetRelativeScale3D();
			float NewScaleY = FMath::FInterpTo(CurrentScale.Y, 1.2f, DeltaTime, GroundScaleInterpSpeed);
			FloorPlate->SetRelativeScale3D(FVector(CurrentScale.X, NewScaleY, CurrentScale.Z));
		}
		else if (WallZ > 10)
		{

			FVector CurrentScale = FloorPlate->GetRelativeScale3D();
			float NewScaleY = FMath::FInterpTo(CurrentScale.Y, 0.0f, DeltaTime, GroundScaleInterpSpeed);
			FloorPlate->SetRelativeScale3D(FVector(CurrentScale.X, NewScaleY, CurrentScale.Z));
		}

		if (WallZ > 60)
		{
			FVector CurrentScale = SkyPlate->GetRelativeScale3D();
			float NewScaleY = FMath::FInterpTo(CurrentScale.Y, 1.2f, DeltaTime, GroundScaleInterpSpeed);
			SkyPlate->SetRelativeScale3D(FVector(CurrentScale.X, NewScaleY, CurrentScale.Z));
		}
		else if (WallZ < 50)
		{
			FVector CurrentScale = SkyPlate->GetRelativeScale3D();
			float NewScaleY = FMath::FInterpTo(CurrentScale.Y, 0.0f, DeltaTime, GroundScaleInterpSpeed);
			SkyPlate->SetRelativeScale3D(FVector(CurrentScale.X, NewScaleY, CurrentScale.Z));
		}


	}

}
