// Copyright (c) 2024, Evelyn Schwab. All rights reserved.


#include "Modifiers/CDCameraModifier_Position_Lag.h"

#include "DrawDebugHelpers.h"
#include "Camera/PlayerCameraManager.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "GameFramework/Pawn.h"

UCDCameraModifier_Position_Lag::UCDCameraModifier_Position_Lag()
{
	InterpSpeedMod = 1.0f;
	bUseMaxDistance = false;
	MaxDistanceBeforeSnap = 100.0f;
	bUseInterpSpeedCurve = false;
	bZeroValueSnaps = true;
	bAddDeltaRotationToInterpSpeed = true;
	DeltaRotationToInterpSpeedScale = 2.0f;
	bVelocityInfluencesRotInterpSpeed = false;
	InterpSpeed = 0.0f;
	CameraPositionTarget = FVector::ZeroVector;
	LaggedCameraPosition = FVector::ZeroVector;
	DistanceToTarget = 0.0f;
	DebugColour = FColor(200, 200, 200, 255);
	DebugTextBaseOffset = FVector2D(150.0f, 25.0f);
	FriendlyName = FText::FromString(TEXT("Position Lag"));
}

void UCDCameraModifier_Position_Lag::AddedToCamera(APlayerCameraManager* Camera)
{
	Super::AddedToCamera(Camera);

	CameraPositionTarget = Camera->GetCameraLocation();
	LaggedCameraPosition = CameraPositionTarget;
	LastFrameRotation = Camera->GetCameraRotation();
}

void UCDCameraModifier_Position_Lag::ModifyCameraBlended(float DeltaTime, FVector ViewLocation, FRotator ViewRotation,
	float FOV, FVector& NewViewLocation, FRotator& NewViewRotation, float& NewFOV)
{
	Super::ModifyCameraBlended(DeltaTime, ViewLocation, ViewRotation, FOV, NewViewLocation, NewViewRotation, NewFOV);
	
	CameraPositionTarget = ViewLocation;
	InterpSpeed = InterpSpeedMod;

	/* Find distance between the previous frame's lagged position and the current frame's position target
	in order to evaluate the interp speed curve */
	if (bUseInterpSpeedCurve)
	{
		FVector DistanceCalcPosition = CameraPositionTarget;
		// Calculate the distance
		DistanceCalcPosition = AxisInfluence.ProcessAxis(LaggedCameraPosition, DistanceCalcPosition);
		
		DistanceToTarget = FVector::Distance(LaggedCameraPosition, DistanceCalcPosition);
		InterpSpeed *= InterpSpeedCurve.GetRichCurveConst()->Eval(DistanceToTarget);
	}

	// Add the delta rotation to the interp speed, if applicable
	if (bAddDeltaRotationToInterpSpeed)
	{
		float RotInterpSpedScale = DeltaRotationToInterpSpeedScale;
		if (bVelocityInfluencesRotInterpSpeed)
		{
			if (const APawn* OwnerPawn = GetOwnerControlledPawn(); IsValid(OwnerPawn))
			{
				const float Velocity = DeltaYawVelocityAxisInfluence.ProcessAxis(FVector::ZeroVector, OwnerPawn->GetVelocity()).Length();
				RotInterpSpedScale *= GetRuntimeFloatCurveValue(DeltaYawVelocityInfluenceCurve, Velocity);
			}
		}
		const float DeltaRot = FMath::RadiansToDegrees(FMath::Acos(LastFrameRotation.Vector() | ViewRotation.Vector()));
		InterpSpeed += DeltaRot * RotInterpSpedScale;
	}
	LastFrameRotation = ViewRotation;
	
	// If the interp speed is zero, and we don't snap at 0, don't interpolate the camera position
	if (!(!bZeroValueSnaps && InterpSpeed <= 0.0f))
	{
		// Interpolate the camera position
		LaggedCameraPosition = FMath::VInterpTo(LaggedCameraPosition, CameraPositionTarget, DeltaTime, InterpSpeed);
	}
	
	LaggedCameraPosition.X = FMath::Lerp(CameraPositionTarget.X, LaggedCameraPosition.X, AxisInfluence.GetXYInfluence());
	LaggedCameraPosition.X = FMath::Lerp(CameraPositionTarget.X, LaggedCameraPosition.X, AxisInfluence.GetXYInfluence());
	
	// Apply the axis influence
	LaggedCameraPosition = AxisInfluence.ProcessAxis(CameraPositionTarget, LaggedCameraPosition);

	// If we are using max distance, clamp the distance to max distance (if the current distance exceeds max distance)
	// This could run regardless of the distance, but we might want to know if we did clamp the distance or not, for debug or other.
	if (bUseMaxDistance)
	{
		const FVector FromOrigin = CameraPositionTarget - LaggedCameraPosition;
		if (DistanceToTarget > MaxDistanceBeforeSnap)
		{
			const FVector ClampedPositionTarget = CameraPositionTarget + FromOrigin.GetClampedToMaxSize(MaxDistanceBeforeSnap);
			LaggedCameraPosition = ClampedPositionTarget;
		}
	}
	
	NewViewLocation = LaggedCameraPosition;
}

void UCDCameraModifier_Position_Lag::DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL,
                                                  float& YPos)
{
	Super::DisplayDebug(Canvas, DebugDisplay, YL, YPos);

	const UFont* DrawFont = GEngine->GetSmallFont();
	int LineNumber = FMath::CeilToInt(YPos / YL);
	Canvas->SetDrawColor(DebugColour);
	
	DrawDebugCanvasWireSphere(Canvas, LaggedCameraPosition, AdjustColourValue(-64, DebugColour), 10.0f, 16);
	DrawDebugCanvasLine(Canvas, LaggedCameraPosition, CameraPositionTarget, AdjustColourValue(-64, DebugColour));
	DrawDebugCanvasWireSphere(Canvas, LaggedCameraPosition, AdjustColourValue(-32, DebugColour), 10.0f, 16);
	DrawDebugTextProjected(Canvas, DrawFont, FString::Printf(TEXT("%i : Lagged camera position"), Priority), LaggedCameraPosition, DebugTextBaseOffset);
	Canvas->DrawText(DrawFont, FString::Printf(TEXT("Distance to target: %f"), DistanceToTarget), 2 * YL, (LineNumber++) * YL);
	Canvas->DrawText(DrawFont, FString::Printf(TEXT("Lagged Target Interp Speed: %f"), InterpSpeed), 2 * YL, (LineNumber++) * YL);

	YPos = LineNumber * YL;
	
}
