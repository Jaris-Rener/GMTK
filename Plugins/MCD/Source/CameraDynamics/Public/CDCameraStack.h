// Copyright (c) 2024, Evelyn Schwab. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CDCameraStack.generated.h"

class UCDCameraModifierInstanced;

/**
 * Data asset that contains a stack of camera modifiers that can be applied to a camera manager. 
 */
UCLASS(BlueprintType, Blueprintable, DisplayName = "Camera Stack")
class CAMERADYNAMICS_API UCDCameraData : public UDataAsset
{
	GENERATED_BODY()

public:

	UCDCameraData();
	
	/*
	 * Array of camera modifiers that will be applied to the camera manager should this data asset become active.
	 * The camera modifiers will be applied in the order they are in the array, unless their priority is overwritten.
	 * 
	 * These modifiers are not used explicitly during runtime, and instead will be copied to the camera manager when the data asset is activated.
	 * This is so that they can be modified during PIE but also be written to from code during runtime.
	 * They should not be written to directly during runtime.
	 *
	 * Note that modifying these during PIE will rebuild that modifier, so any runtime changes will be lost.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, Category = "Camera Dynamics", meta = (ShowInnerProperties))
	TArray<TObjectPtr<UCDCameraModifierInstanced>> CameraModifiers;
};
