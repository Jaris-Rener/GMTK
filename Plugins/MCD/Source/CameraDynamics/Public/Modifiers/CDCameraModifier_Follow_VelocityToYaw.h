// Copyright (c) 2024, Evelyn Schwab. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modifiers/CDCameraModifier_Instanced.h"
#include "CDCameraModifier_Follow_VelocityToYaw.generated.h"

UENUM(BlueprintType)
enum EFollowCamPitchInfluence
{
	Pitch_Ignored			UMETA(DisplayName = "Ignored"),
	Pitch_Influenced		UMETA(DisplayName = "Influenced"),
	Pitch_Static			UMETA(DisplayName = "Static Target")
};

/**
 * 
 */
UCLASS(DisplayName = "Camera Modifier - Follow - Orientation from Velocity")
class CAMERADYNAMICS_API UCDCameraModifier_Follow_VelocityToYaw : public UCDCameraModifierInstanced
{
	GENERATED_BODY()

public:

	UCDCameraModifier_Follow_VelocityToYaw();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera Dynamics", meta = (InlineEditConditionToggle))
	bool bWaitForNoInput;

	/** The base influence the velocity has on the rotation */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera Dynamics")
	float InterpolationSpeed;
	
	/** Curve for how much influence the velocity has on the rotation, with the x-axis being the time since the last player input */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera Dynamics",
		meta = (EditCondition = "bWaitForNoInput", DisplayName = "Interpolation Speed from Time Since Input"))
	FRuntimeFloatCurve InfluenceTimeSinceInput;
	
	/** Interp speed from the difference between the current and target rotation */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera Dynamics",
		meta = (DisplayName = "Interpolation Speed from Distance to Target"))
	FRuntimeFloatCurve InfluenceTargetToCurrentDifference;

	/** Base threshold for the time since the last player input */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera Dynamics", meta = (EditCondition = "bWaitForNoInput"))
	float MinTimeSinceRotationInput;
	
	/** Dictates how the pitch is treated when the camera is following the velocity */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera Dynamics")
	TEnumAsByte<EFollowCamPitchInfluence> PitchInfluenceType;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera Dynamics")
	FVector VelocityScale;
	
	/** If the pitch is static, this is the value it will be set to */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera Dynamics",
		meta = (EditCondition = "PitchInfluenceType == EFollowCamPitchInfluence::Pitch_Static"))
	float StaticPitchValue;
	
	/** Curve for how much influence the velocity has on the yaw, with the x-axis being the pawn's velocity */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera Dynamics")
	FRuntimeFloatCurve VelocityYawInfluence;

	/** Offsets applied to the target velocity rotation, most useful for pitch values */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera Dynamics")
	FRotator VelocityRotationOffset;

	
protected:

	virtual bool ProcessViewRotationBlended(AActor* ViewTarget, float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot) override;
	
	virtual void DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos) override;
private:

	FRotator RotationInput;
	FVector PawnVelocityVector;
	FRotator PawnVelocityRotator;
	float TimeSinceLastInput;
	float TrueInterpSpeed;
};
