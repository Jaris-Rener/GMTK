// Copyright (c) 2024, Evelyn Schwab. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CDCameraModifier_Instanced.h"
#include "Data/CameraDynamicDataTypes.h"
#include "CDCameraModifier_Position_Lag.generated.h"

/**
 * Modifier for adding lag to the camera position.
 */
UCLASS(DisplayName = "Camera Modifier - Position - Position Lag")
class CAMERADYNAMICS_API UCDCameraModifier_Position_Lag : public UCDCameraModifierInstanced
{
	GENERATED_BODY()

public:
	
	UCDCameraModifier_Position_Lag();
	
	/** Basic interp speed for the camera target */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Dynamics")
	float InterpSpeedMod;

	/** The axis this lag is applied to */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Dynamics")
	FCDCameraAxisData AxisInfluence;
	
	/** Max distance the camera can be from the target before it snaps */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Dynamics", meta = (EditCondition = "bUseMaxDistance"))
	float MaxDistanceBeforeSnap;

	/** If we utilize the max lag distance */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Dynamics", meta = (InlineEditConditionToggle))
	bool bUseMaxDistance;
	
	/** Do we use an interp speed curve */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Dynamics", meta = (InlineEditConditionToggle))
	bool bUseInterpSpeedCurve;
	
	/** If this is false,a zero value for the interpolation speed will not snap the camera to the target. Instead, the camera will not move at all. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Dynamics")
	bool bZeroValueSnaps;
	
	/** Float curve, interp speed / distance between the position and position target */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Dynamics", meta = (EditCondition = "bUseInterpSpeedCurve"))
	FRuntimeFloatCurve InterpSpeedCurve;

	/** Should the delta rotation of the camera be added to the interp speed of the lag */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Dynamics|Delta Rotation", meta = (InlineEditConditionToggle))
	bool bAddDeltaRotationToInterpSpeed;

	/** Scale of the delta rotation to the interp speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Dynamics|Delta Rotation", meta = (EditCondition = "bAddDeltaRotationToInterpSpeed"))
	float DeltaRotationToInterpSpeedScale;

	/** Should the velocity of the player influence the rotation interpolation speed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Dynamics|Delta Rotation", meta = (InlineEditConditionToggle))
	bool bVelocityInfluencesRotInterpSpeed;

	/** Multiplicative influence of the velocity of the character on the rotation interp speed. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Dynamics|Delta Rotation", meta = (EditCondition = "bVelocityInfluencesRotInterpSpeed"))
	FRuntimeFloatCurve DeltaYawVelocityInfluenceCurve;

	/** The influence that the delta yaw of the camera has on the lag interpolation speed. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Dynamics|Delta Rotation", meta = (EditCondition = "bVelocityInfluencesRotInterpSpeed"))
	FCDCameraAxisData DeltaYawVelocityAxisInfluence;
	
protected:

	virtual void AddedToCamera(APlayerCameraManager* Camera) override;
	virtual void ModifyCameraBlended(float DeltaTime, FVector ViewLocation, FRotator ViewRotation, float FOV,
	                                 FVector& NewViewLocation, FRotator& NewViewRotation, float& NewFOV) override;
	virtual void DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos) override;
	
private:

	float InterpSpeed;
	float DistanceToTarget;
	FVector CameraPositionTarget;
	FVector LaggedCameraPosition;
	FRotator LastFrameRotation;
};
