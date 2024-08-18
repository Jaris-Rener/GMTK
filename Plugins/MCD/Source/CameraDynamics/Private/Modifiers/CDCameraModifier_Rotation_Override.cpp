// Copyright (c) 2024, Evelyn Schwab. All rights reserved.


#include "Modifiers/CDCameraModifier_Rotation_Override.h"

#include "DrawDebugHelpers.h"
#include "Camera/PlayerCameraManager.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"

UCDCameraModifier_Rotation_Override::UCDCameraModifier_Rotation_Override()
{
	RotationOverride = FRotator::ZeroRotator;
	RotationOverrideType = CAMROT_None;
	LookAtActor = nullptr;
	LookAtComponent = nullptr;
	
	DebugColour = FColor::Black;
	FriendlyName = FText::FromString(TEXT("Rotation Override"));
}

bool UCDCameraModifier_Rotation_Override::ProcessViewRotationBlended(AActor* ViewTarget, float DeltaTime,
                                                                     FRotator& OutViewRotation, FRotator& OutDeltaRot)
{
	FRotator TargetRotation = FRotator::ZeroRotator;
	FRotator TargetDeltaRotation = FRotator::ZeroRotator;

	switch (RotationOverrideType)
	{
		case CAMROT_Absolute:
			TargetRotation = RotationOverride;
			break;
		case CAMROT_LookAtLocation:
			TargetRotation = (LookAtLocation - CameraOwner->GetCameraLocation()).Rotation();
			break;
		case CAMROT_LookAtActor:
			if (IsValid(LookAtActor))
			{
				TargetRotation = (LookAtActor->GetActorLocation() - CameraOwner->GetCameraLocation()).Rotation();
			}
			break;
		case CAMROT_SceneComponent:
			if (IsValid(LookAtComponent))
			{
				TargetRotation = (LookAtComponent->GetComponentLocation() - CameraOwner->GetCameraLocation()).Rotation();
			}
			break;
		default:
			return false;
	}
	
	OutViewRotation = TargetRotation;
	OutDeltaRot = TargetDeltaRotation;
	return false;
}

void UCDCameraModifier_Rotation_Override::DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay,
	float& YL, float& YPos)
{
	Super::DisplayDebug(Canvas, DebugDisplay, YL, YPos);

	const UFont* DrawFont = GEngine->GetSmallFont();
	int LineNumber = FMath::CeilToInt(YPos / YL);

	FVector TargetLocation;
	FString DebugString;

	// Find the appropriate debug string and debug location
	switch (RotationOverrideType)
	{
	case CAMROT_Absolute:
		DebugString = FString::Printf(TEXT("Rotation Override: %s"), *RotationOverride.ToCompactString());
		break;
	case CAMROT_LookAtLocation:
		DebugString = FString::Printf(TEXT("Look At Location: %s"), *LookAtLocation.ToCompactString());
		TargetLocation = LookAtLocation;
		break;
	case CAMROT_LookAtActor:
		if (IsValid(LookAtActor))
		{
			DebugString = FString::Printf(TEXT("Look At Actor: %s"), *LookAtActor->GetName());
			TargetLocation = LookAtActor->GetActorLocation();
			break;
		}
		DebugString = FString::Printf(TEXT("Look At Actor: None"));
		break;
	case CAMROT_SceneComponent:
		if (IsValid(LookAtComponent))
		{
			DebugString = FString::Printf(TEXT("Look At Component: %s"), *LookAtComponent->GetName());
			TargetLocation = LookAtComponent->GetComponentLocation();
			break;
		}
		DebugString = FString::Printf(TEXT("Look At Component: None"));
		break;
	default:
		break;
	}

	// Draw the debug string and location
	Canvas->SetDrawColor(FColor::Red);
	Canvas->DrawText(DrawFont, DebugString, 2 * YL, (LineNumber++) * YL);
	
	if (RotationOverrideType != CAMROT_Absolute && RotationOverrideType != CAMROT_None)
	{
		DrawDebugTextProjected(Canvas, DrawFont, FString::Printf(TEXT("%i : Look At Target"), Priority), TargetLocation, DebugTextBaseOffset);
		DrawDebugCanvasWireSphere(Canvas, TargetLocation, FColor::Red, 10.0f, 16);
	}

	YPos = LineNumber * YL;
}
