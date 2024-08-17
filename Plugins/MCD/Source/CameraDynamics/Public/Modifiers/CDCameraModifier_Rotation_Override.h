// Copyright (c) 2024, Evelyn Schwab. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modifiers/CDCameraModifier_Instanced.h"
#include "CDCameraModifier_Rotation_Override.generated.h"


/**
 * Rotation override types for camera rotation override modifiers.
 */
UENUM(BlueprintType, Category = "Camera Dynamics|Modifiers|Rotation")
enum ECDRotationOverrideType
{
	CAMROT_None				UMETA(DisplayName = "None"),
	CAMROT_Absolute			UMETA(DisplayName = "Absolute Rotation"),
	CAMROT_LookAtLocation	UMETA(DisplayName = "Look At Location"),
	CAMROT_LookAtActor		UMETA(DisplayName = "Look At Actor"),
	CAMROT_SceneComponent	UMETA(DisplayName = "Look At Scene Component")
};

/**
 * Override the camera's rotation to a specific value, or towards a specific target point/actor.
 * If using LookAtLocation or LookAtActor, this modifier needs to be after any location modifiers in the stack.
 */
UCLASS(DisplayName = "Camera Modifier - Rotation - Override")
class CAMERADYNAMICS_API UCDCameraModifier_Rotation_Override : public UCDCameraModifierInstanced
{
	GENERATED_BODY()

public:

	UCDCameraModifier_Rotation_Override();

	/** Type of override to apply */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Dynamics")
	TEnumAsByte<ECDRotationOverrideType> RotationOverrideType;

	/** Absolute rotation override */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera Dynamics",
		meta = (EditCondition = "RotationOverrideType == ECDRotationOverrideType::CAMROT_Absolute", EditConditionHides))
	FRotator RotationOverride;

	/** Rotation look at location */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera Dynamics",
		meta = (EditCondition = "RotationOverrideType == ECDRotationOverrideType::CAMROT_LookAtLocation",
			EditConditionHides))
	FVector LookAtLocation;

	/** Rotation look at target actor */
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Camera Dynamics",
		meta = (EditCondition = "RotationOverrideType == ECDRotationOverrideType::CAMROT_LookAtActor",
			EditConditionHides))
	AActor* LookAtActor;

	/** Rotation look at target component */
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Camera Dynamics",
		meta = (EditCondition = "RotationOverrideType == ECDRotationOverrideType::CAMROT_SceneComponent",
			EditConditionHides))
	USceneComponent* LookAtComponent;
	
protected:

	virtual bool ProcessViewRotationBlended(AActor* ViewTarget, float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot) override;

	virtual void DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos) override;
};
