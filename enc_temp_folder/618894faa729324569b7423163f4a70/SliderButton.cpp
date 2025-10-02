#include "SliderButton.h"
#include "BrickSpacePawn.h"

#include "Selector.h"

USliderButton::USliderButton()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void USliderButton::BeginPlay()
{
    Super::BeginPlay();

    if (StartCollider)
    {
        StartCollider->OnComponentBeginOverlap.AddDynamic(this, &USliderButton::OnOverlapBegin);
        StartCollider->OnComponentEndOverlap.AddDynamic(this, &USliderButton::OnOverlapEnd);
    }

    if (LaunchCollider)
    {
        LaunchCollider->OnComponentBeginOverlap.AddDynamic(this, &USliderButton::OnOverlapBegin);
        LaunchCollider->OnComponentEndOverlap.AddDynamic(this, &USliderButton::OnOverlapEnd);
    }
}
void USliderButton::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    ABrickSpacePawn* BrickPawn = Cast<ABrickSpacePawn>(OtherActor);
    if (!BrickPawn || !timer) return;

    if (OverlappedComp == StartCollider)
    {
        /*Press();*/
        timer->StartTimer(BrickPawn);
    }
    else if (OverlappedComp == LaunchCollider)
    {
        /*Press();*/
        timer->StopTimer(BrickPawn);
    }
}

void USliderButton::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    ABrickSpacePawn* BrickPawn = Cast<ABrickSpacePawn>(OtherActor);
    if (!BrickPawn) return;

    /*Release();*/
}
//void USliderButton::Focus(USelector* cursor, bool state)
//{
//   
//    Super::Focus(cursor, state);
//    
//    ABrickSpacePawn* BrickPawn = nullptr;
//
//    if (cursor)
//    {
//        AActor* OwnerActor = cursor->GetOwner();
//        if (OwnerActor)
//        {
//            BrickPawn = Cast<ABrickSpacePawn>(OwnerActor);
//        }
//    }
//
//    if (!LaunchButton && !StartButton) return;
//
//    if (state)
//    {   
//        if (LaunchButton) 
//        {
//            Press();
//            if (timer) 
//            {
//                timer->StopTimer(BrickPawn);
//            }
//        }
//        else if (StartButton)
//        {
//            Press();
//            if (timer)
//            {
//                timer->StartTimer(BrickPawn);
//            }
//        }
//    }
//    else
//    {     
//        Release();
//    }
//}

//void USliderButton::Press()
//{
//    UE_LOG(LogTemp, Log, TEXT("%s pressed!"), *GetName());
//    OnPressed.Broadcast();
//}
//
//void USliderButton::Release()
//{
//
//
//    UE_LOG(LogTemp, Log, TEXT("%s released!"), *GetName());
//    OnReleased.Broadcast(); 
//}

void USliderButton::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}