// Copyright (c) 2024, Evelyn Schwab. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/CameraDynamicDataTypes.h"
#include "Modifiers/CDCameraModifier_Instanced.h"
#include "CDCameraModifier_FOV_PitchMod.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "Camera Modifier - FOV - Pitch Modifier")
class CAMERADYNAMICS_API UCDCameraModifier_FOV_PitchMod : public UCDCameraModifierInstanced
{
	GENERATED_BODY()

public:

	UCDCameraModifier_FOV_PitchMod();

	
	/** Curve for applying an FOV change to the camera pitch */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Dynamics")
    FCameraCurveModData PitchToFOVData;
	
	/** Should the pitch be remapped to a different range before applying the FOV change curve */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Dynamics")
    bool bRemapPitch;
	
	/** The in range for the pitch */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Dynamics", meta = (EditCondition = "bRemapPitch", EditConditionHides))
	FFloatRange PitchRange;
	
	/** The out range for the pitch */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Dynamics", meta = (EditCondition = "bRemapPitch", EditConditionHides))
	FFloatRange PitchOutRange;
	

		
protected:

	virtual void ModifyCameraBlended(float DeltaTime, FVector ViewLocation, FRotator ViewRotation, float FOV,
	                                 FVector& NewViewLocation, FRotator& NewViewRotation, float& NewFOV) override;
	
	virtual void DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos) override;

private:

	float InFOV;
	float OutFOV;
	
	bool bWarnedForNonFloatCurve;
};

