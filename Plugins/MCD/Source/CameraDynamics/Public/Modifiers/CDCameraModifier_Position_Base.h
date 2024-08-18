// Copyright (c) 2024, Evelyn Schwab. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/CameraDynamicDataTypes.h"
#include "Modifiers/CDCameraModifier_Instanced.h"
#include "CDCameraModifier_Position_Base.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "Camera Modifier - Position - Base")
class CAMERADYNAMICS_API UCDCameraModifier_Position_Base : public UCDCameraModifierInstanced
{
	GENERATED_BODY()

public:

	UCDCameraModifier_Position_Base();
	
	/** The base position for the camera */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera Dynamics", meta = (FullyExpand))
	FCameraSourcePositionData CameraBasePosition;

	/** The axis influence of this modifier */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera Dynamics", meta = (FullyExpand))
	FCDCameraAxisData AxisInfluence;
	
protected:
	
	virtual void ModifyCameraBlended(float DeltaTime, FVector ViewLocation, FRotator ViewRotation, float FOV,
	                                 FVector& NewViewLocation, FRotator& NewViewRotation, float& NewFOV) override;

	virtual void DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos) override;
	
private:

	/*
	 * Camera positional data
	 * These values could be scoped to ModifyCamera, but they're useful for drawing debug info
	 */

	FVector CameraInitialPosition;
	
};
