// Copyright (c) 2024, Evelyn Schwab. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modifiers/CDCameraModifier_Instanced.h"
#include "CDCameraModifier_Position_VelocityOffset.generated.h"

UENUM(BlueprintType)
enum ELagSpace
{
	Space_World			UMETA(DisplayName = "World"),
	Space_Relative		UMETA(DisplayName = "Relative")
};

/**
 * Modifier to offset the camera's position based on the player's velocity.
 */
UCLASS(DisplayName = "Camera Modifier - Position - Offset from Velocity")
class CAMERADYNAMICS_API UCDCameraModifier_Position_VelocityOffset : public UCDCameraModifierInstanced
{
	GENERATED_BODY()

public:

	UCDCameraModifier_Position_VelocityOffset();

	/** Scale of the velocity offset relative to the velocity itself */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera Dynamics")
	FVector VelocityOffsetScale;

	/** If a curve is not used, the linear scale will be used. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera Dynamics", meta = (InlineEditConditionToggle))
    bool bUseOffsetCurve;
	
	/** Multiplier for velocity to offset of the camera. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera Dynamics")
	float LinearVelocityScale;
	
	/** Velocity length / offset curve */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera Dynamics", meta = (DisplayThumbnail = false, EditCondition = "bUseOffsetCurve"))
	FRuntimeFloatCurve VelocityOffsetCurve;

	/** If a curve is not used, the interp speed float will be used */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera Dynamics", meta = (InlineEditConditionToggle))
	bool bUseInterpSpeedCurve;
	
	/** Interpolation speed for the velocity offset. 0 will be instant, this is a form of camera lag. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera Dynamics")
	float VelocityOffsetInterpSpeedScale;

	/** Velocity length / Interpolation speed curve */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera Dynamics", meta = (DisplayThumbnail = false, EditCondition = "bUseInterpSpeedCurve"))
	FRuntimeFloatCurve InterpSpeedCurve;

	/** The space the velocity offset is applied in. Local space can cause inconstant motion, but is more reactive. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera Dynamics")
	bool bWorldSpace;
	
protected:

	virtual void ModifyCameraBlended(float DeltaTime, FVector ViewLocation, FRotator ViewRotation, float FOV,
	                                 FVector& NewViewLocation, FRotator& NewViewRotation, float& NewFOV) override;
	
	virtual void DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos) override;

private:
	static FVector RotateVectorFromActor(TObjectPtr<AActor> InActor, const FVector& InVector);
	static FVector UnRotateVectorFromActor(TObjectPtr<AActor> InActor, const FVector& InVector);
	
	FVector UnmodifiedPosition; // The position before the camera modifier is applied, used for debug display
	FVector VelocityOffset;	// the real velocity offset
	FVector VelocityOffsetTarget; // the velocity offset we are interpolating to
};
