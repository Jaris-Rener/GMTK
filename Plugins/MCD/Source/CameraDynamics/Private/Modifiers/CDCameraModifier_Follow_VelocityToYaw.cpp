// Copyright (c) 2024, Evelyn Schwab. All rights reserved.


#include "Modifiers/CDCameraModifier_Follow_VelocityToYaw.h"

#include "DrawDebugHelpers.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

UCDCameraModifier_Follow_VelocityToYaw::UCDCameraModifier_Follow_VelocityToYaw()
{
	bWaitForNoInput = true;
	InterpolationSpeed = 1.0f;
	StaticPitchValue = 15.0f;
	PitchInfluenceType = Pitch_Static;
	TimeSinceLastInput = 0.0f;
	TrueInterpSpeed = 0.0f;
	MinTimeSinceRotationInput = 2.5f;
	VelocityScale = FVector(1.0f, 1.0f, 0.0f);
	
	DebugColour = FColor::Magenta;
}

bool UCDCameraModifier_Follow_VelocityToYaw::ProcessViewRotationBlended(AActor* ViewTarget, float DeltaTime,
	FRotator& OutViewRotation, FRotator& OutDeltaRot)
{
	TrueInterpSpeed = InterpolationSpeed; // Base interp speed
	
	//---------- Time since last input ----------
	
	// Get the time since the last controller input
	RotationInput = GetOwnerController()->RotationInput;
	if (!RotationInput.IsNearlyZero())
	{
		TimeSinceLastInput = 0.0f;
		TrueInterpSpeed = 0.0f;
		return false;
	}
	TimeSinceLastInput += DeltaTime;
	
	// ---------- Calculate the target and interp speed ----------

	// Get the interp speed modifier from the time since the last rotation input
	if (bWaitForNoInput)
	{
		TrueInterpSpeed *= GetRuntimeFloatCurveValue(InfluenceTimeSinceInput, TimeSinceLastInput - MinTimeSinceRotationInput);
		if (TrueInterpSpeed <= 0.0f) return false;
	}
	
	const APawn* OwnerControlledPawn = GetOwnerControlledPawn();
	if (!OwnerControlledPawn) return false;
	PawnVelocityVector = OwnerControlledPawn->GetVelocity() * VelocityScale;
	PawnVelocityRotator = PawnVelocityVector.Rotation();
	
	// Early return if the velocity is zero
	if (PawnVelocityVector.IsNearlyZero()) return false;
	
	TrueInterpSpeed *= GetRuntimeFloatCurveValue(VelocityYawInfluence, PawnVelocityVector.Length());
	if (TrueInterpSpeed <= 0.0f) return false;
	
	// Handle the pitch influence
	switch (PitchInfluenceType)
	{
		case Pitch_Ignored:
			PawnVelocityRotator.Pitch = OutViewRotation.Pitch;
			break;
		case Pitch_Static:
			PawnVelocityRotator.Pitch = StaticPitchValue;
			break;
		default:
			break;
	}
	
	PawnVelocityRotator += VelocityRotationOffset;				// Apply the target rotation offsets

	// Multiply the interp speed the difference in rotation between the current view rotation and the target velocity rotation
	const float CurrentToTargetDifference = FMath::RadiansToDegrees(
    	FMath::Acos(PawnVelocityRotator.Vector() | OutViewRotation.Vector()));
	TrueInterpSpeed *= GetRuntimeFloatCurveValue(InfluenceTargetToCurrentDifference, CurrentToTargetDifference);
	if (TrueInterpSpeed <= 0.0f) return false;
	
	// ---------- Perform the interpolation ----------

	const FRotator TargetRotation = FMath::RInterpTo(OutViewRotation, PawnVelocityRotator, DeltaTime, TrueInterpSpeed);
	OutDeltaRot = TargetRotation - OutViewRotation;
	return false;
}

void UCDCameraModifier_Follow_VelocityToYaw::DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay,
                                                          float& YL, float& YPos)
{
	Super::DisplayDebug(Canvas, DebugDisplay, YL, YPos);
	
	const UFont* DrawFont = GEngine->GetSmallFont();
	int LineNumber = FMath::CeilToInt(YPos / YL);
	
	// Debug 3D info
	if (!PawnVelocityVector.IsNearlyZero() && !(TrueInterpSpeed <= 0.0f))
	{
		const FVector ArrowStart = GetOwnerControlledPawn()->GetActorLocation();
		const FVector ArrowEndVector = ArrowStart + PawnVelocityVector.GetSafeNormal() * 100.0f;
		DrawDebugDirectionalArrow(GetWorld(), ArrowStart, ArrowEndVector, 10.0, AdjustColourValue(-64, DebugColour));
		Canvas->SetDrawColor(AdjustColourValue(-64, DebugColour));
		DrawDebugTextProjected(Canvas, DrawFont, FString::Printf(TEXT("%i Pawn true velocity"), Priority), ArrowEndVector);
		
		const FVector ArrowEndRotator = ArrowStart + PawnVelocityRotator.Vector() * 100;
		DrawDebugDirectionalArrow(GetWorld(), ArrowStart, ArrowEndRotator, 10.0f, DebugColour);
		Canvas->SetDrawColor(DebugColour);
		DrawDebugTextProjected(Canvas, DrawFont, FString::Printf(TEXT("%i Pawn adjusted target velocity"), Priority), ArrowEndRotator);
	}
	
	// Debug print info
	Canvas->DrawText(DrawFont, FString::Printf(TEXT("Current rotation input: %s"), *RotationInput.ToCompactString()),
				 2 * YL, (LineNumber++) * YL);
	Canvas->DrawText(DrawFont, FString::Printf(TEXT("Time since last detected rotation input: %f"), TimeSinceLastInput),
	                 2 * YL, (LineNumber++) * YL);
	Canvas->DrawText(DrawFont, FString::Printf(TEXT("Pawn velocity rotator: %s"), *PawnVelocityRotator.ToCompactString()),
				 2 * YL, (LineNumber++) * YL);
	Canvas->DrawText(DrawFont, FString::Printf(TEXT("Velocity rotation interpolation speed: %f"), TrueInterpSpeed),
					 2 * YL, (LineNumber++) * YL);
	
	YPos = LineNumber * YL;
}
