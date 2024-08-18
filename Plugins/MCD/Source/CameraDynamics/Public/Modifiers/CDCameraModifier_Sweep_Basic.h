// Copyright (c) 2024, Evelyn Schwab. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/CameraDynamicDataTypes.h"
#include "Modifiers/CDCameraModifier_Instanced.h"
#include "CDCameraModifier_Sweep_Basic.generated.h"


/**
 * Struct to hold data for a basic camera trace.
 */
USTRUCT(BlueprintType, Category = "Camera Dynamics")
struct FCameraTraceData
{
	GENERATED_BODY()

	/** Source position for the camera trace - the trace start */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera Dynamics")
	FCameraSourcePositionData TraceStartPoint;

	/** The radius of the sphere trace. */
	UPROPERTY(BlueprintReadWrite , EditAnywhere, Category = "Camera Dynamics")
	float TraceRadius;

	/** The channel to use for the trace. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera Dynamics")
	TEnumAsByte<ECollisionChannel> TraceChannel;
	
	FCameraTraceData()
	{
		TraceRadius = 10.0f;
		TraceChannel = ECC_Camera;
	}
};

/**
 * Basic line trace to prevent the camera from clipping through objects.
 */
UCLASS(DisplayName = "Camera Modifier - Sweep - Basic Sweep")
class CAMERADYNAMICS_API UCDCameraModifier_Sweep_Basic : public UCDCameraModifierInstanced
{
	GENERATED_BODY()

public:

	UCDCameraModifier_Sweep_Basic();

	/** Data for the camera trace */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera Dynamics", meta = (FullyExpand))
	FCameraTraceData CameraTraceData;
	
protected:

	virtual void ModifyCamera(float DeltaTime, FVector ViewLocation, FRotator ViewRotation, float FOV,
	                          FVector& NewViewLocation, FRotator& NewViewRotation, float& NewFOV) override;

	virtual void DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos) override;

private:

	FVector TraceStart;
	FVector TraceEnd;
	FVector TraceHit;
	
};
