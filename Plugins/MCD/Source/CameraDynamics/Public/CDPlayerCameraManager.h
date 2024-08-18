// Copyright (c) 2024, Evelyn Schwab. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "CDCameraStack.h"
#include "GameplayTagContainer.h"
#include "Camera/PlayerCameraManager.h"
#include "CDPlayerCameraManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnViewTargetChangeStart, AActor*, NewViewTarget, FViewTargetTransitionParams, TransitionParams);

class UCDCameraData;
class UCDCameraModifierInstanced;

/**
 * 
 */
UCLASS()
class CAMERADYNAMICS_API ACDPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:

	ACDPlayerCameraManager(const FObjectInitializer& ObjectInitializer);

	virtual void InitializeFor(APlayerController* PC) override;
	
	/**
	 * Adds a specified camera data to the camera manager, blending in all the instanced camera modifiers.
	 * @param NewCameraData - The camera data to add to the camera manager.
	 */
	UFUNCTION(BlueprintCallable, Category = "Camera Dynamics")
	void AddCameraData(UCDCameraData* NewCameraData);

	/**
	 * Removes a specific camera data from the camera manager, blending out all the instanced camera modifiers.
	 * @param CameraData - The camera data to remove from the camera manager.
	 * @return - True if the camera data was successfully removed.
	 */
	UFUNCTION(BlueprintCallable, Category = "Camera Dynamics")
	bool RemoveCameraData(UCDCameraData* CameraData);

	/**
	 * Removes all camera data from the camera manager, blending out all the instanced camera modifiers.
	 */
	UFUNCTION(BlueprintCallable, Category = "Camera Dynamics")
	void RemoveAllCameraData();
	
	/**
	 * The default camera data to be applied when this camera modifier is initialized.
	 * Does not need to be valid, as camera data can be applied at runtime.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera Dynamics")
	TObjectPtr<UCDCameraData> DefaultCameraData;

	
	/**
	 * Get all the active camera data on this camera manager.
	 * @return - An array of all the active camera data.
	 */
	UFUNCTION(BlueprintPure, Category = "Camera Dynamics")
	TArray<UCDCameraData*> GetActiveCameraData() const { return CameraDataList; }
	
	/**
	 * Get the first active modifier of this type.
	 * @param ModifierClass - The class of the modifier to get. 
	 * @param RequiredTags - Any tags to require on the modifier. Will not search for tags if this is empty.
	 * @return - Any valid found modifiers.
	 */
	UFUNCTION(BlueprintPure, Category = "Camera Dynamics", meta = (DeterminesOutputType = "ModifierClass", AdvancedDisplay = "RequiredTags", AutoCreateRefTerm = "RequiredTags"))
	UCameraModifier* GetActiveModifierOfClass(const TSubclassOf<UCameraModifier> ModifierClass, UPARAM() const FGameplayTagContainer& RequiredTags);

	/**
	 * Get all active modifiers of this type.
	 * @param ModifierClass - The class of the modifier to get. 
	 * @param RequiredTags - Any tags to require on the modifier. Will not search for tags if this is empty.
	 * @return - All the valid found modifiers.
	 */
	UFUNCTION(BlueprintPure, Category = "Camera Dynamics", meta = (DeterminesOutputType = "ModifierClass", AdvancedDisplay = "RequiredTags", AutoCreateRefTerm = "RequiredTags"))
	TArray<UCameraModifier*> GetAllActiveModifiersOfClass(const TSubclassOf<UCameraModifier>& ModifierClass, const FGameplayTagContainer& RequiredTags);

	/**
	* If true, rotation inputs (from the player and any camera modifiers) will be combined with UCameraDynamicsFunctionLibrary::OrientationAwareRotationComposition.
	* This is to prevent the camera from accumulating roll during modifier in/out blends.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Dynamics")
	bool bUseOrientationAwareRotationComposition;

	// We are fully overriding this function to change the way in which rotation are being blended to account for orientation.
	virtual void ProcessViewRotation(float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot) override;
	
	virtual void SetViewTarget(AActor* NewViewTarget, FViewTargetTransitionParams TransitionParams) override;

	UPROPERTY(BlueprintAssignable, Category = "Camera Dynamics")
	FOnViewTargetChangeStart OnViewTargetChangeStart;
	
private:

	/** The camera data currently being used by the camera manager. */
	UPROPERTY() TArray<TObjectPtr<UCDCameraData>> CameraDataList;
};
