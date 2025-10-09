#include "Selector.h"
#include "Vodget.h"

// Sets default values for this component's properties
USelector::USelector() 
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

void USelector::ForePinch(const bool Value)
{
	if (focusVodget != nullptr)
		focusVodget->ForePinch(this, Value);
}
void USelector::MiddlePinch(const bool Value)
{
	if (focusVodget != nullptr)
		focusVodget->MiddlePinch(this, Value);
}
void USelector::Grip(const bool Value)
{
	if (focusVodget != nullptr)
		focusVodget->Grip(this, Value);
}

// Inheriting selectors must all maintain a model of a 3D cursor in world coordinates. 
const FTransform& USelector::Cursor()
{
	// Inheriting selectors must all override SetCursor() to set FTransform cursor based on the selection method. 
	SetCursor();
	return cursor;
}

void USelector::SetFilter(uint16 filter)
{
	selectionFilter = filter;
}

UVodget* USelector::GetFocusVodget()
{
    return focusVodget;
}

UVodget* USelector::GetFocusGrabbedVodget()
{
	if (focus_grabbed)
		return focusVodget;
	return nullptr;
}


// This method should be pure virtual but is implemented to avoid Unreal compiler issues.
void USelector::SetCursor() {}
