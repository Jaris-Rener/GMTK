// Copyright (c) 2024, Evelyn Schwab. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/HitResult.h"
#include "Modifiers/CDCameraModifier_Instanced.h"
#include "CDCameraModifier_Position_DynamicZ.generated.h"

/**
 * Modifies the camera's position based on the character's Z position. Mainly makes it so that the camera doesn't
 * follow the character's Z when jumping, unless they have gained a given amount of height.
 */
UCLASS(DisplayName = "Camera Modifier - Position - Dynamic Z")
class CAMERADYNAMICS_API UCDCameraModifier_Position_DynamicZ : public UCDCameraModifierInstanced
{
	GENERATED_BODY()

public:

	UCDCameraModifier_Position_DynamicZ();

	/** The interpolation speed to use while airborne */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera Dynamics")
	FRuntimeFloatCurve AirborneInterpSpeed;


	/**
	 * 	Distance threshold at which to snap the camera to the character's Z position.
	 * 	This should never actually be used, and acts as a fail-safe. The interpolation speed should reach high enough
	 * 	to prevent this distance from being reached.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera Dynamics")
	float SnapThreshold;

	/** The speed at which the Z position should return to the player when returning to a grounded position */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera Dynamics")
	FRuntimeFloatCurve ReturnSpeed;
	
	UFUNCTION()
	void OnOwnerPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);
	
	UFUNCTION()
	void OnCharacterLanded(const FHitResult& Hit);
protected:
	
	virtual void AddedToCamera(APlayerCameraManager* Camera) override;

	virtual void ModifyCameraBlended(float DeltaTime, FVector ViewLocation, FRotator ViewRotation, float FOV, FVector& NewViewLocation, FRotator& NewViewRotation, float& NewFOV) override;

	virtual void RemoveSelfFromModifierList() override;
	
	virtual void DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos) override;
	
private:
	
	FVector LastGroundedPosition;
	FVector CurrentPosition;
	FVector TargetPosition;
	bool bShouldDirectInterpZ;
	
};
