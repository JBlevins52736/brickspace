// Fill out your copyright notice in the Description page of Project Settings.


#include "Calib2Pt.h"

void UCalib2Pt::SetLocalCursor()
{
	if (posAnchorSet) {
		cursorsrt.SetLocation(posAnchor);
		if (rotAnchorSet) {
			FVector xAxis = rotAnchor - posAnchor;
			xAxis.Z = 0.0;
			FQuat rot = FRotationMatrix::MakeFromXZ(xAxis, FVector::UpVector).ToQuat();
			cursorsrt.SetRotation(rot);
		}
	}
	else {
		Super::SetLocalCursor();
	}
}

void UCalib2Pt::UpdatePosAnchor(FVector pos)
{
	UE_LOG(LogTemp, Warning, TEXT("UpdatePosAnchor receiving"));

	if (rotAnchorSet) {

		rotAnchorSet = false; // Don't rotate when resetting position.
		posAnchorSet = false; // Reset childsrt.
	}


	posAnchor = pos;
	if (posAnchorSet) {
		UpdateCursors();
	}
	else {
		posAnchorSet = true;
		SetLocalCursor();
		childsrt = cursorsrt.Inverse();
	}
}

void UCalib2Pt::UpdateRotAnchor(FVector pos)
{
	if (!posAnchorSet)
		return;

	rotAnchor = pos;
	if (rotAnchorSet)
	{
		UpdateCursors();
	}
	else {
		rotAnchorSet = true;
		SetLocalCursor();
		childsrt = cursorsrt.Inverse();
	}
}

void UCalib2Pt::EnableToggle(bool val)
{
	// Show/hide the calibration points
}