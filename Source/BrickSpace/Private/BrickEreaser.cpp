#include "BrickEreaser.h"

void UBrickEreaser::BeginPlay()
{
    Super::BeginPlay();
    selectionFilter = 0x04; 
}

void UBrickEreaser::ForePinch(USelector* selector, bool state)
{
    if (state && brickTouched != nullptr)
    {
        UBrick* brick = Cast<UBrick>(brickTouched);
        if (brick != nullptr)
        {
            UStaticMeshComponent* mesh = Cast<UStaticMeshComponent>(brick->GetAttachParent());
            if (mesh != nullptr && mesh->Mobility == EComponentMobility::Movable)
            {
                brickTouched = nullptr;

                brick->GetOwner()->Destroy();
            }
        }
    }
}