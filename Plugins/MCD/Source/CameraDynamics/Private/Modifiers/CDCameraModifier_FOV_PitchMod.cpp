// Copyright (c) 2024, Evelyn Schwab. All rights reserved.


#include "Modifiers/CDCameraModifier_FOV_PitchMod.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"

UCDCameraModifier_FOV_PitchMod::UCDCameraModifier_FOV_PitchMod()
{
	PitchToFOVData.CurveEvaluationType = ECameraCurveModType::ECM_Additive;
	bRemapPitch = false;
	PitchRange = FFloatRange(-160.0f, -25.0f);
	PitchOutRange = FFloatRange(0.0f, 1.0f);

	DebugColour = FColor::Emerald;
	FriendlyName = FText::FromString(TEXT("Pitch Driven FOV Modifier"));
	InFOV = 0.0f;
	OutFOV = 0.0f;
	bWarnedForNonFloatCurve = false;
}

void UCDCameraModifier_FOV_PitchMod::ModifyCameraBlended(float DeltaTime, FVector ViewLocation, FRotator ViewRotation,
	float FOV, FVector& NewViewLocation, FRotator& NewViewRotation, float& NewFOV)
{
	Super::ModifyCameraBlended(DeltaTime, ViewLocation, ViewRotation, FOV, NewViewLocation, NewViewRotation, NewFOV);

	InFOV = FOV;
	
	float Pitch = NewViewRotation.Pitch;

	if (Pitch > 90.0f)
	{
		Pitch = 0;
	}
	if (bRemapPitch)
	{
		Pitch = FMath::GetMappedRangeValueClamped(PitchRange, PitchOutRange, Pitch);
	}
	
	const float EvaluatedCurveValue = PitchToFOVData.Curve.GetRichCurveConst()->Eval(Pitch);

	switch (PitchToFOVData.CurveEvaluationType)
	{
	case ECM_Absolute:
		NewFOV = EvaluatedCurveValue;
		break;
	case ECM_Additive:
		NewFOV += EvaluatedCurveValue;
		break;
	case ECM_Multiplicative:
		NewFOV *= EvaluatedCurveValue;
		break;
	default:
		break;
	}
	
	OutFOV = NewFOV;
}

void UCDCameraModifier_FOV_PitchMod::DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL,
                                                  float& YPos)
{
	Super::DisplayDebug(Canvas, DebugDisplay, YL, YPos);

	const UFont* DrawFont = GEngine->GetSmallFont();
	int LineNumber = FMath::CeilToInt(YPos / YL);
	
	Canvas->DrawText(DrawFont, FString::Printf(TEXT("Incoming FOV = %f | Modified FOV = %f"), InFOV, OutFOV), 2 * YL, (LineNumber++) * YL);

	YPos = LineNumber * YL;
}
