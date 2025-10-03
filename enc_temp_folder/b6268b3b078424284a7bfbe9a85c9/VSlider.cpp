#include "VSlider.h"

void UVSlider::Focus(USelector* cursor, bool state)
{
	// Optional: Change some property of the clients material to highlight when focused. 

	UVodget::Focus(cursor, state);


	if (UMeshComponent* MeshComp = Cast<UMeshComponent>(clientComponent))
	{
		if (state)
		{
			// Apply highlight overlay
			MeshComp->SetOverlayMaterial(HighlightOverlayMaterial);
		}
		else
		{
			// Remove highlight overlay
			MeshComp->SetOverlayMaterial(nullptr);
		}
	}

}

void UVSlider::ForePinch(USelector* selector, bool state)
{
	// ToDo: On Grab true, convert the world cursors position into local client space and
	// save the initialgrabval of the axis along which the slider knob moves.
	// Don't forget to grab and release selector focus appropriately.
	if (state) // Begin grab
	{
		if (selector)
		{
			grabbingSelector = selector;

			// Use selector cursor transform
			FVector worldPos = selector->Cursor().GetLocation();

			// Convert into local slider space
			FVector localPos = GetComponentTransform().InverseTransformPosition(worldPos);

			// Save initial grab value along Z
			initialgrabval = localPos.Z;

			selector->GrabFocus(true);

			// Remove highlight overlay when grabbed
			if (UMeshComponent* MeshComp = Cast<UMeshComponent>(clientComponent))
			{
				MeshComp->SetOverlayMaterial(nullptr);
			}
		}
	}
	else // End grab
	{
		if (grabbingSelector)
		{
			grabbingSelector->GrabFocus(false);
			grabbingSelector = nullptr;
		}
	}
}

void UVSlider::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	// ToDo: While grabbing operation is active, calculate the current value of the world cursor along the 
	// local movement axis. Move the client along this local axis so that the current value matches 
	// the initial value again. 

	// Once the knob is following the cursor correctly along the movement axis, clamp the motion to keep
	// the knob inside its slot.

	// Once the knob is moving correctly within its slot calculate the percentage that the knob is between
	// its minimum and maximum value and broadcast the percentage to all callbacks registered to SliderDelegate.
	// e.g.: SliderDelegate.Broadcast(pct);
	if (grabbingSelector)
	{
		// Current cursor world position
		FVector worldPos = grabbingSelector->Cursor().GetLocation();

		// Convert into local space
		FVector localPos = GetComponentTransform().InverseTransformPosition(worldPos);

		// handle Z
		float currZ = localPos.Z;

		float errorZ = currZ - initialgrabval;

		FVector loc = clientComponent->GetRelativeLocation();
		loc.Z += errorZ;
		loc.Z = FMath::Clamp(loc.Z, minval, maxval);
		clientComponent->SetRelativeLocation(loc);
		
		float pct = (loc.Z - minval) / (maxval - minval);
		SliderDelegate.Broadcast(pct);
			
		
	}
}

