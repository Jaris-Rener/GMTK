// Copyright (c) 2024, Evelyn Schwab. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/CameraDynamicDataTypes.h"
#include "Modifiers/CDCameraModifier_Instanced.h"
#include "CDCameraModifier_Position_Offset.generated.h"

/**
 * Modifer that offsets the camera's position.
 */
UCLASS(DisplayName = "Camera Modifier - Position - Offset")
class CAMERADYNAMICS_API UCDCameraModifier_Position_Offset : public UCDCameraModifierInstanced
{
	GENERATED_BODY()

public:

	UCDCameraModifier_Position_Offset();

	/** The position offsets to apply */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera Dynamics")
	FCameraOffsetPositionData CameraOffsetPosition;
	
protected:

	virtual void ModifyCameraBlended(float DeltaTime, FVector ViewLocation, FRotator ViewRotation, float FOV, FVector& NewViewLocation, FRotator& NewViewRotation, float& NewFOV) override;
	
	virtual void DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos) override;

	
private:

	FVector UnmodifiedPosition;
	FVector ModifiedPosition;
};
