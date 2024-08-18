// Copyright (c) 2024, Evelyn Schwab. All rights reserved.


#include "Modifiers/CDCameraModifier_Position_Base.h"

#include "DrawDebugHelpers.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "GameFramework/Pawn.h"

UCDCameraModifier_Position_Base::UCDCameraModifier_Position_Base()
{
	DebugColour = FColor::Orange;
	FriendlyName = FText::FromString(TEXT("Base Position"));
}

void UCDCameraModifier_Position_Base::ModifyCameraBlended(float DeltaTime, FVector ViewLocation, FRotator ViewRotation,
	float FOV, FVector& NewViewLocation, FRotator& NewViewRotation, float& NewFOV)
{
	Super::ModifyCameraBlended(DeltaTime, ViewLocation, ViewRotation, FOV, NewViewLocation, NewViewRotation, NewFOV);
	
	APawn* OwnerControlledPawn = GetOwnerControlledPawn();
	if (!IsValid(OwnerControlledPawn)) { return; }
	
	// Get the camera's base position
	FVector PotentialViewLocation = CameraBasePosition.FindSourcePosition(OwnerControlledPawn);

	// Apply axis influence
	NewViewLocation = AxisInfluence.ProcessAxis(NewViewLocation, PotentialViewLocation);
	
	CameraInitialPosition = NewViewLocation;
}

void UCDCameraModifier_Position_Base::DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL,
                                                   float& YPos)
{
	Super::DisplayDebug(Canvas, DebugDisplay, YL, YPos);

	const UFont* DrawFont = GEngine->GetSmallFont();
	int LineNumber = FMath::CeilToInt(YPos / YL);

	
	DrawDebugCanvasWireSphere(Canvas, CameraInitialPosition, DebugColour, 2.5f, 16);
	Canvas->SetDrawColor(DebugColour);
	Canvas->DrawText(
		DrawFont, FString::Printf(TEXT("Camera initial position: %s"), *CameraInitialPosition.ToCompactString()),
		2 * YL, (LineNumber++) * YL);
	
	YPos = LineNumber * YL;
}
