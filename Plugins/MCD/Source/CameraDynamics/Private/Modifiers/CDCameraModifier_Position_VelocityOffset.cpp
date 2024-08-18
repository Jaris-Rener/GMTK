// Copyright (c) 2024, Evelyn Schwab. All rights reserved.


#include "Modifiers/CDCameraModifier_Position_VelocityOffset.h"

#include "DrawDebugHelpers.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "GameFramework/Pawn.h"

UCDCameraModifier_Position_VelocityOffset::UCDCameraModifier_Position_VelocityOffset()
{
	VelocityOffsetScale = FVector(1.0f, 1.0f, 0.0f);
	LinearVelocityScale = 1.0f;
	bUseOffsetCurve = false;
	bWorldSpace = false;
	VelocityOffsetInterpSpeedScale = 0.0f;
	bUseInterpSpeedCurve = false;
	DebugColour = FColor::Cyan;
	FriendlyName = FText::FromString(TEXT("Velocity-Driven Offset"));
}

void UCDCameraModifier_Position_VelocityOffset::ModifyCameraBlended(float DeltaTime, FVector ViewLocation,
	FRotator ViewRotation, float FOV, FVector& NewViewLocation, FRotator& NewViewRotation, float& NewFOV)
{
	Super::ModifyCameraBlended(DeltaTime, ViewLocation, ViewRotation, FOV, NewViewLocation, NewViewRotation, NewFOV);
	
	UnmodifiedPosition = NewViewLocation;
	const APawn* OwnerPawn = GetOwnerControlledPawn();
	if (!IsValid(OwnerPawn)) return;
	
	// Get the target position of the velocity offset, either from the scale or curve
	FVector PawnVelocity = OwnerPawn->GetVelocity();
	
	if (bUseOffsetCurve)
	{
		VelocityOffsetTarget = (PawnVelocity * VelocityOffsetScale).GetSafeNormal() * GetRuntimeFloatCurveValue(
			VelocityOffsetCurve, (PawnVelocity * VelocityOffsetScale).Length());
	}
	else
	{
		VelocityOffsetTarget = PawnVelocity * LinearVelocityScale;
	}

	VelocityOffsetTarget *= VelocityOffsetScale;
	
	// Make the velocity relative if not in world space
	if (!bWorldSpace) VelocityOffsetTarget = OwnerPawn->GetActorRotation().UnrotateVector(VelocityOffsetTarget);

	// Interpolate the velocity offset
	float InterpSpeed = VelocityOffsetInterpSpeedScale;

	if (bUseInterpSpeedCurve)
	{
		InterpSpeed *= InterpSpeedCurve.GetRichCurveConst()->Eval(PawnVelocity.Length());
	}
	
	VelocityOffset = FMath::VInterpTo(VelocityOffset, VelocityOffsetTarget, DeltaTime, InterpSpeed);
	// Re-rotate the velocity if not in world space
	if (!bWorldSpace)
	{
		NewViewLocation += OwnerPawn->GetActorRotation().RotateVector(VelocityOffset);
		return;
	}
	NewViewLocation += VelocityOffset;
}

void UCDCameraModifier_Position_VelocityOffset::DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay,
                                                             float& YL, float& YPos)
{
	Super::DisplayDebug(Canvas, DebugDisplay, YL, YPos);

	const UFont* DrawFont = GEngine->GetSmallFont();
	int LineNumber = FMath::CeilToInt(YPos / YL);
	Canvas->SetDrawColor(DebugColour);

	FVector DebugVelocityOffset;
	if (bWorldSpace)
	{
		DebugVelocityOffset = UnmodifiedPosition + VelocityOffset;
	}
	else
	{
		DebugVelocityOffset =  UnmodifiedPosition + RotateVectorFromActor(GetOwnerControlledPawn(), VelocityOffset);
	}
	
	DrawDebugCanvasWireSphere(Canvas, DebugVelocityOffset, DebugColour, 10.0f, 16);
	DrawDebugTextProjected(Canvas, DrawFont, FString::Printf(TEXT("%i : Velocity offset"), Priority), DebugVelocityOffset, DebugTextBaseOffset);
	Canvas->DrawText(DrawFont, FString::Printf(TEXT("Velocity offset target: %s"), *VelocityOffsetTarget.ToCompactString()), 2 * YL, (LineNumber++) * YL);

	if (VelocityOffsetInterpSpeedScale > 0.0f)
	{
		FVector DebugVelocityOffsetTarget;
		if (bWorldSpace)
		{
			DebugVelocityOffsetTarget = UnmodifiedPosition + VelocityOffsetTarget;
		}
		else
		{
			DebugVelocityOffsetTarget = UnmodifiedPosition + RotateVectorFromActor(GetOwnerControlledPawn(), VelocityOffsetTarget);
		}
		const FColor TargetColour = AdjustColourValue(-100, DebugColour);
		Canvas->SetDrawColor(TargetColour);
		DrawDebugCanvasWireSphere(Canvas, DebugVelocityOffsetTarget, TargetColour, 10.0f, 16);
		DrawDebugTextProjected(Canvas, DrawFont, FString::Printf(TEXT("%i : Velocity offset target"), Priority),
		                       DebugVelocityOffsetTarget, DebugTextBaseOffset + FVector2D(0.0f, -50.0f));
		DrawDebugCanvasLine(Canvas, DebugVelocityOffset, DebugVelocityOffsetTarget, TargetColour);
		Canvas->DrawText(
			DrawFont, FString::Printf(
				TEXT("Lagged velocity offset: %s"), *VelocityOffset.ToCompactString()), 2 * YL,
			(LineNumber++) * YL);
	}
	

	YPos = LineNumber * YL;
}

FVector UCDCameraModifier_Position_VelocityOffset::RotateVectorFromActor(const TObjectPtr<AActor> InActor, const FVector& InVector)
{
	if (!IsValid(InActor)) return FVector::ZeroVector;
	return InActor->GetActorRotation().RotateVector(InVector);
}

FVector UCDCameraModifier_Position_VelocityOffset::UnRotateVectorFromActor(const TObjectPtr<AActor> InActor, const FVector& InVector)
{
	if (!IsValid(InActor)) return FVector::ZeroVector;
	return InActor->GetActorRotation().UnrotateVector(InVector);
}
