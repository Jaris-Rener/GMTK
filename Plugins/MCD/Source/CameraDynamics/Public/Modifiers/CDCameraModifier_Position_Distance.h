// Copyright (c) 2024, Evelyn Schwab. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/CameraDynamicDataTypes.h"
#include "Modifiers/CDCameraModifier_Instanced.h"
#include "CDCameraModifier_Position_Distance.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "Camera Modifier - Position - Distance")
class CAMERADYNAMICS_API UCDCameraModifier_Position_Distance : public UCDCameraModifierInstanced
{
	GENERATED_BODY()

public:

	UCDCameraModifier_Position_Distance();

	/** The target distance for this offset, in the camera's X vector */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera Dynamics")
	float TargetDistance;

	/** Should changes in the target distance value be smoothed */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera Dynamics|Smoothing", meta = (InlineEditConditionToggle))
	bool bSmoothDistanceChanges;

	/* The interpolation speed for distance changes, if smoothing is enabled */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera Dynamics|Smoothing", meta = (EditCondition = "bSmoothDistanceChanges"))
	float ChangeSmoothing;
	
private:

	float Distance;
	
protected:

	virtual void AddedToCamera(APlayerCameraManager* Camera) override;
	virtual void ModifyCameraBlended(float DeltaTime, FVector ViewLocation, FRotator ViewRotation, float FOV, FVector& NewViewLocation, FRotator& NewViewRotation, float& NewFOV) override;
	
};
