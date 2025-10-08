#include "SliderButton.h"
#include "BrickSpacePawn.h"

#include "Selector.h"

USliderButton::USliderButton()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = false;

}

void USliderButton::BeginPlay()
{
    Super::BeginPlay();
}

void USliderButton::Focus(USelector* cursor, bool state)
{
    Super::Focus(cursor, state);

    if (!clientComponent || !cursor) return;

    if (state)
    {
        PrimaryComponentTick.SetTickFunctionEnable(state);
        focusSelector = cursor;

        FVector CursorWorldLocation = focusSelector->Cursor().GetLocation();

        // 1. Get the cursor's Z position in the client component's local space
        float CursorLocalLocationZ = clientComponent->GetComponentTransform().InverseTransformPosition(CursorWorldLocation).Z;

        // 2. Store the current Z position of the button component (This is the "0" point)
        InitialLocalLocation = clientComponent->GetRelativeLocation().Z;

        // 3. Store the initial offset between the cursor and the button
        // This is crucial for stable movement: CursorLocalLocationZ = InitialLocalLocation + InitialCursorOffsetZ
        InitialCursorOffsetZ = CursorLocalLocationZ - InitialLocalLocation;
    }
    else
    {
        // Disable ticking and reset button to its starting Z-position
        PrimaryComponentTick.SetTickFunctionEnable(state);
        if (clientComponent)
        {
            FVector OriginalLocation = clientComponent->GetRelativeLocation();
            OriginalLocation.Z = InitialLocalLocation;  // Reset to the stored starting Z
            clientComponent->SetRelativeLocation(OriginalLocation);
        }
    }

    isPressed = false;
    // ... (Your BLAH code block is omitted for brevity as it was disabled)
}



void USliderButton::Press()
{
    UE_LOG(LogTemp, Log, TEXT("%s pressed!"), *GetName());
    OnPressed.Broadcast();
    isPressed = true;
}


void USliderButton::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!focusSelector)
        return;

    // 1. Get current cursor Z in button's local space
    FVector CursorWorldLocation = focusSelector->Cursor().GetLocation();
    float CursorLocalLocationZ = clientComponent->GetComponentTransform().InverseTransformPosition(CursorWorldLocation).Z;

    // 2. Calculate the desired Z position by subtracting the initial offset.
    // New Z = Cursor Z - Initial Offset. This creates the stable follow behavior.
    float DesiredZ = CursorLocalLocationZ - InitialCursorOffsetZ;

    // 3. Clamp the movement. Clamp between MaxDepressionDistance (most negative) and InitialLocalLocation (the starting position, typically 0.0f).
    DesiredZ = FMath::Clamp(DesiredZ, MaxDepressionDistance, InitialLocalLocation);

    // 4. Apply the new clamped location.
    FVector NewLocalLocation = clientComponent->GetRelativeLocation();
    NewLocalLocation.Z = DesiredZ;
    clientComponent->SetRelativeLocation(NewLocalLocation);


    // 5. Check for press based on the total depression distance.
    // Depression = Starting Z - Current Z (this gives a positive value for movement into negative Z)
    float CurrentDepression = DesiredZ - InitialLocalLocation;


    // Note: Assuming PressThreshold is now a positive value (e.g., 3.0f)
    if (CurrentDepression <= PressThreshold && !isPressed)
    {

        Press();
        if (timer)
        {
            ABrickSpacePawn* BrickPawn = Cast<ABrickSpacePawn>(focusSelector->GetOwner());
            if (!BrickPawn) return;

            // Check which component the Vodget is attached to (clientComponent)
            // You might need a more robust way to distinguish these buttons if they all use the same USliderButton class
            // attached to different meshes, but this is based on your original logic.
            if (clientComponent == StartButton)
            {
                UE_LOG(LogTemp, Log, TEXT("StartButton pressed!"));
                timer->StartTimer(BrickPawn);

            }
            else if (clientComponent == LaunchButton)
            {
                UE_LOG(LogTemp, Log, TEXT("LaunchButton pressed!"));
                timer->StopTimer(BrickPawn);
            }
            else if (clientComponent == ResetButton)
            {
                UE_LOG(LogTemp, Log, TEXT("ResetButton pressed!"));
                timer->ResetTimer(BrickPawn);
            }

            UE_LOG(LogTemp, Warning, TEXT("Depression: %f"), CurrentDepression);
        }
    }
}