// Fill out your copyright notice in the Description page of Project Settings.


#include "FocusVodget.h"

void UFocusVodget::Focus(USelector* cursor, bool state)
{
	UVodget::Focus(cursor, state);
	pressed.Broadcast(state);
	
}
