// Copyright (c) 2024, Evelyn Schwab. All rights reserved.


#include "Modifiers/CDCameraModifier_Position_Offset.h"
#include "DrawDebugHelpers.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"

UCDCameraModifier_Position_Offset::UCDCameraModifier_Position_Offset()
{
	DebugColour = FColor::Yellow;
	FriendlyName = FText::FromString(TEXT("Position Offset"));
}

void UCDCameraModifier_Position_Offset::ModifyCameraBlended(float DeltaTime, FVector ViewLocation,
	FRotator ViewRotation, float FOV, FVector& NewViewLocation, FRotator& NewViewRotation, float& NewFOV)
{
	Super::ModifyCameraBlended(DeltaTime, ViewLocation, ViewRotation, FOV, NewViewLocation, NewViewRotation, NewFOV);

	UnmodifiedPosition = ViewLocation;
    ModifiedPosition = CameraOffsetPosition.GetOffsetPosition(ViewLocation, ViewRotation);
    NewViewLocation = ModifiedPosition;
}

void UCDCameraModifier_Position_Offset::DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL,
                                                     float& YPos)
{
	Super::DisplayDebug(Canvas, DebugDisplay, YL, YPos);

	const UFont* DrawFont = GEngine->GetSmallFont();
	const int LineNumber = FMath::CeilToInt(YPos / YL);
	Canvas->SetDrawColor(DebugColour);

	DrawDebugCanvasLine(Canvas, UnmodifiedPosition, ModifiedPosition, DebugColour);
	DrawDebugCanvasWireSphere(Canvas, ModifiedPosition, DebugColour, 2.5f, 16);
	DrawDebugTextProjected(Canvas, DrawFont, FString::Printf(TEXT("%i : Camera offset position"), Priority), ModifiedPosition, DebugTextBaseOffset);
	
	YPos = LineNumber * YL;
}
