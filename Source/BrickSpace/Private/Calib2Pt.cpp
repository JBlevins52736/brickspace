// Fill out your copyright notice in the Description page of Project Settings.


#include "Calib2Pt.h"

void UCalib2Pt::ActiveChanged()
{
	Super::ActiveChanged();

	// Resetting enables normal world grabbing until pos anchor is set.
	posAnchorSet = false;
	rotAnchorSet = false;
}

void UCalib2Pt::SetLocalCursor()
{
	if (posAnchorSet) {

		if (rotAnchorSet && GetOwner()->HasAuthority()) 
		{
			//ChangeWorldScaling(posAnchor, rotAnchor);
		}

		//FVector posAnchorScaled = posAnchorInit * (currWorldToMeters / initWorldToMeters);

		//FTransform worldsrt = childsrt * cursorsrt;
		//FTransform pawnChildOfWorld = GetRelativeTransform() * worldsrt.Inverse();

	//	posAnchorMarker->SetWorldLocation(posAnchorScaled);


		cursorsrt.SetLocation(posAnchor);
		if (rotAnchorSet) {
			FVector xAxis = rotAnchor - posAnchor;
			xAxis.Z = 0.0;
			FQuat rot = FRotationMatrix::MakeFromXZ(xAxis, FVector::UpVector).ToQuat();
			cursorsrt.SetRotation(rot);
		}
	}
	else {
		// Revert to normal world grabbing until posAnchor is set.
		Super::SetLocalCursor();
	}
}

void UCalib2Pt::UpdatePosAnchor(FVector pos)
{
	if (rotAnchorSet) {
		// Reset and start pos anchor grabbing
		posAnchorSet = false;
		rotAnchorSet = false;
	}

	posAnchor = pos;

	//posAnchorInit = pos;
	//initWorldToMeters = currWorldToMeters;
	//posAnchorLocal = GetComponentTransform().InverseTransformPosition(pos);

	if (posAnchorSet) {
		// Update pos anchor grabbing.
		UpdateCursors();
	}
	else {
		// Start grabbing the pos anchor
		posAnchorSet = true;
		SetLocalCursor();
		childsrt = cursorsrt.Inverse();
	}
}

void UCalib2Pt::UpdateRotAnchor(FVector pos)
{
	// Rotation needs pos anchor to set first.
	if (!posAnchorSet)
		return;

	rotAnchor = pos;
	if (rotAnchorSet)
	{
		// Update rot anchor (bimanual) grabbing.
		UpdateCursors();
	}
	else {
		// Start grabbing the rot anchor (same as beginning bimanual world grabbing)
		rotAnchorSet = true;
		//if (GetOwner()->HasAuthority()) {
		//	StartWorldScaling(posAnchor, rotAnchor);
		//}
		SetLocalCursor();
		childsrt = cursorsrt.Inverse();
	}
}
