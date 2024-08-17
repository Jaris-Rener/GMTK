// Copyright (c) 2024, Evelyn Schwab. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modifiers/CDCameraModifier_Instanced.h"
#include "CDCameraModifier_FOV_Adjust.generated.h"

/**
 * Simple camera modifier to adjust the FOV of the camera
 */
UCLASS(DisplayName = "Camera Modifier - FOV - Adjustment")
class CAMERADYNAMICS_API UCDCameraModifier_FOV_Adjust : public UCDCameraModifierInstanced
{
	GENERATED_BODY()
	
public:

	UCDCameraModifier_FOV_Adjust();

	/** Default value for the FOV change */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera Dynamics")
	float DefaultFOVChange;

	/** Runtime value for the FOV change */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Camera Dynamics")
    float TargetFOVChange;

	/** Should the FOV change be smoothed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Dynamics|Smoothing", meta = (InlineEditConditionToggle))
	bool bUseSmoothing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Dynamics|Smoothing", meta = (EditCondition = "bUseSmoothing"))
	float SmoothingSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera Dynamics")
	TEnumAsByte<ECameraModOpType> ModificationType;

private:
	
	float FOVChange;
	float ChangedFOV;	// Saved for debugging purposes
protected:

	virtual void AddedToCamera(APlayerCameraManager* Camera) override;
	
	virtual void ModifyCameraBlended(float DeltaTime, FVector ViewLocation, FRotator ViewRotation, float FOV, FVector& NewViewLocation, FRotator& NewViewRotation, float& NewFOV) override;

	virtual void DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos) override;
	
#if WITH_EDITOR
	
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

#endif
	
};
