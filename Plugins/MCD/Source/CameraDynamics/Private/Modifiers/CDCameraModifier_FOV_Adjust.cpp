// Copyright (c) 2024, Evelyn Schwab. All rights reserved.


#include "Modifiers/CDCameraModifier_FOV_Adjust.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"

UCDCameraModifier_FOV_Adjust::UCDCameraModifier_FOV_Adjust()
{
	DefaultFOVChange = 90.0f;
	TargetFOVChange = DefaultFOVChange;
	FOVChange = TargetFOVChange;
	ChangedFOV = FOVChange;
	ModificationType = CMO_Absolute;
	bUseSmoothing = false;
	SmoothingSpeed = 2.0f;
	DebugColour = FColor::Turquoise;
	FriendlyName = FText::FromString(TEXT("FOV Adjustment"));
}

void UCDCameraModifier_FOV_Adjust::AddedToCamera(APlayerCameraManager* Camera)
{
	Super::AddedToCamera(Camera);

	TargetFOVChange = DefaultFOVChange;	// Set the target FOV change to the default value, required for smoothing to blend
	FOVChange = TargetFOVChange;
}

void UCDCameraModifier_FOV_Adjust::ModifyCameraBlended(float DeltaTime, FVector ViewLocation, FRotator ViewRotation,
                                                       float FOV, FVector& NewViewLocation, FRotator& NewViewRotation, float& NewFOV)
{
	Super::ModifyCameraBlended(DeltaTime, ViewLocation, ViewRotation, FOV, NewViewLocation, NewViewRotation, NewFOV);

	if (bUseSmoothing)
	{
		FOVChange = FMath::FInterpTo(FOVChange, TargetFOVChange, DeltaTime, SmoothingSpeed);
	}
	else FOVChange = TargetFOVChange;
	
	switch (ModificationType)	// Apply the FOV change based on the modification type
    {
    	case CMO_Absolute:
    		ChangedFOV = FOVChange;
    		break;
    	case CMO_Additive:
    		ChangedFOV = FOV + FOVChange;
    		break;
    	case CMO_Multiplicative:
    		ChangedFOV = FOV * FOVChange;
    		break;
    	default:
    		break;
    }
	NewFOV = ChangedFOV;	// Set the new FOV, ChangedFOV is used as a debug value
}

void UCDCameraModifier_FOV_Adjust::DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL,
	float& YPos)
{
	Super::DisplayDebug(Canvas, DebugDisplay, YL, YPos);

	const UFont* DrawFont = GEngine->GetSmallFont();
	int LineNumber = FMath::CeilToInt(YPos / YL);
	Canvas->SetDrawColor(DebugColour);

	const FString FOVString = FString::Printf(TEXT("FOV Change: %f, %s | New FOV = %f"), FOVChange, *UEnum::GetValueAsString(ModificationType), ChangedFOV);
	Canvas->DrawText(DrawFont, FOVString, 2 * YL, (LineNumber++) * YL);
	YPos = LineNumber * YL;
}

#if WITH_EDITOR

// Called when a property is changed in the editor, used to update the target FOV change value
void UCDCameraModifier_FOV_Adjust::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property == nullptr) return;
	
	if (PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(UCDCameraModifier_FOV_Adjust, DefaultFOVChange))
	{
		TargetFOVChange = DefaultFOVChange;
	}
}

#endif