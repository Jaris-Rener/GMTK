// Copyright (c) 2024, Evelyn Schwab. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Camera/CameraModifier.h"
#include "Camera/PlayerCameraManager.h"
#include "Engine/Canvas.h"
#include "Runtime/Engine/Classes/Curves/CurveFloat.h"
#include "CDCameraModifier_Instanced.generated.h"


/**
 * Enum for the debug level of the camera modifier
 */
UENUM(BlueprintType, Category = "Camera Dynamics|Debug")
enum EDebugLevel
{
	DL_Light		UMETA(DisplayName = "Light"),
	DL_Verbose		UMETA(DisplayName = "Verbose"),
};

/**
 * Generic enum for a camera modifier operation type
 */
UENUM(BlueprintType, Category = "Camera Dynamics|Modifiers")
enum ECameraModOpType
{
	CMO_Absolute			UMETA(DisplayName = "Absolute"),
	CMO_Additive			UMETA(DisplayName = "Additive"),
	CMO_Multiplicative		UMETA(DisplayName = "Multiplicative")
};

class UCDCameraData;
class ACharacter;

/**
 * Camera modifier class marked as EditInlineNew and DefaultToInstanced. Parent class for all instanced camera modifiers.
 */
UCLASS(EditInlineNew, DefaultToInstanced, Abstract , AutoExpandCategories = ("Camera Dynamics"))
class CAMERADYNAMICS_API UCDCameraModifierInstanced : public UCameraModifier 
{
	GENERATED_BODY()

public:

	UCDCameraModifierInstanced();
	
	/** Gameplay tags owned by this camera modifier, used when searching through the camera manager's list of modifiers */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera Dynamics")
	FGameplayTagContainer ModifierGameplayTags;
	
	/** If this value is false, use the 'priority' variable. Otherwise, use the position of the modifier in the array */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "CameraModifier")
	bool bUseCustomPriority;

	/**
	* Dictates how intense the debug visuals are, to prevent the debug HUD from being too cluttered.
	* This value does nothing unless implemented, so won't be active on all camera modifiers.
	*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Debug")
	TEnumAsByte<EDebugLevel> DebugLevel;
	
	/** Camera data asset that this modifier is sourced from */
	TWeakObjectPtr<UCDCameraData> CameraDataSource;

	/** Name for this modifier that will be shown in the simplified editor and in some debug displays */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	FText FriendlyName;

	/** Use a custom blending curve for blending the modifier in */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraModifier|Blend", meta = (InlineEditConditionToggle))
	bool bUseCustomBlendIn;

	/** Use a custom blending curve for blending the modifier out */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraModifier|Blend", meta = (InlineEditConditionToggle))
	bool bUseCustomBlendOut;

	/** Custom blend curve for blending the modifier in. This must be between 0 and 1. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraModifier|Blend", meta = (EditCondition = "bUseCustomBlendIn"))
	FRuntimeFloatCurve CustomBlendIn;
	
	/** Custom blend curve for blending the modifier out. This must be between 0 and 1. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraModifier|Blend", meta = (EditCondition = "bUseCustomBlendOut"))
	FRuntimeFloatCurve CustomBlendOut;

	/**
	 * If this is true, the modifier will blend out automatically when the view target changes to one with a matching tag.
	 * This is mainly useful for sequencer cameras - Give you sequencer camera a matching tag, and the camera modifier will
	 * Blend automatically in and out with the same blend time as the blend in sequencer.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CameraModifier|Blend")
	bool bBlendOutForViewTargetWithMatchingTag;

	/**
	 * The tag that the view target must have for this modifier to blend out automatically.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "CameraModifier|Blend", meta = (EditCondition = "bBlendOutForViewTargetWithMatchingTag"))
	FName ViewTargetBlendOutTag;

	/**
	 * Should this modifier bind to changes in the view target of the player controller?
	 * This is mainly used for blending out when the view target changes to one with a matching tag
	 * as per ViewTargetBlendOutTag.
	 * Disabling this will prevent that from working, but will very marginally improve performance by not checking view targets.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CameraModifier|Blend")
	bool bWatchForViewTargetChange;
	
	virtual void UpdateAlpha(float DeltaTime) override;

	/** Start a timer to remove this modifier after the blend out time */
	virtual void MarkForRemoval();
	
	/** Runtime version of this camera modifier. */
	TWeakObjectPtr<UCDCameraModifierInstanced> RuntimeModifier;
	
protected:

	/** Default debug display info. Also handles the bool bDrawDebugInfoThisFrame for the next frame */
	virtual void DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos) override;
	
	/*
	 * Helper functions
	 */
	
	/** Get the pawn controlled by the owning controller. May not be valid.	*/
	UFUNCTION(BlueprintPure, Category = "Camera Dynamics")
	APawn* GetOwnerControlledPawn() const;

	/** Get the character controlled by the owning controller. May not be valid. */
	UFUNCTION(BlueprintPure, Category = "Camera Dynamics")
	ACharacter* GetOwnerControlledCharacter() const;

	/** Get the player controller that owns the camera manager associated with this modifier */
	UFUNCTION(BlueprintPure, Category = "Camera Dynamics")
	APlayerController* GetOwnerController() const;
	
	/** Returns true if the camera debugger is open (ShowDebug Camera) */
	UFUNCTION(BlueprintPure, Category = "Camera Dynamics")
	bool ShouldDrawDebug() const { return bDrawDebugInfoThisFrame; }
	
	/** Colour that this modifier will use for debug drawing */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Debug")
	FColor DebugColour;

	// Offset for the debug text
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Debug", AdvancedDisplay)
	FVector2D DebugTextBaseOffset;

	UFUNCTION()
	virtual void AddedToCamera(APlayerCameraManager* Camera) override;
	
	
	virtual void EnableModifier() override;
	
	virtual auto ModifyCamera(float DeltaTime, FVector ViewLocation, FRotator ViewRotation, float FOV,
	                          FVector& NewViewLocation, FRotator& NewViewRotation, float& NewFOV) -> void override;
	
	/**
	 * Triggers when the camera modifier is added to the camera
	 * @param Camera The player camera manager that this modifier was added to
	 */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Camera Dynamics")
	void BlueprintAddedToCamera(APlayerCameraManager* Camera);
	
	// Modifies the camera the same as ModifyCamera, but is automatically blended with the alpha value
	virtual void ModifyCameraBlended(float DeltaTime, FVector ViewLocation, FRotator ViewRotation, float FOV,
							  FVector& NewViewLocation, FRotator& NewViewRotation, float& NewFOV);


	/**
	 * Blueprint level blended camera modification.
	 * This function does not account for rotation inputs or anything handled in ProcessViewRotation.
	 * As such, it is recommended to use ModifyCameraBlended instead for rotation changes.
	 * 
	 * @param BlendAlpha - The alpha value for the blend. Keep in bind that blends are handled in the camera manager,
	 * so this value should not need to be used for anything other than custom blending.
	 * @param ViewLocation - The view location of the camera at the start of this modifier's application.
	 * @param ViewRotation - The view rotation of the camera at the start of this modifier's application.
	 * @param FOV - The FOV of the camera at the start of this modifier's application.
	 * @param NewViewLocation - The new view location to be applied by this camera modifier.
	 * @param NewViewRotation - The new view rotation to be applied by this camera modifier.
	 * @param NewFOV - The new FOV to be applied by this camera modifier.
	 */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Camera Dynamics")
	void BlueprintModifyCameraBlended(float BlendAlpha, float DeltaTime, FVector ViewLocation, FRotator ViewRotation, float FOV,
							  FVector& NewViewLocation, FRotator& NewViewRotation, float& NewFOV);

	virtual bool ProcessViewRotation(AActor* ViewTarget, float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot) override;
	
	/**
	 * Use this function to handle view rotation changes that will be applied to the camera.
	 * Unless you have a specific reason not to, use UCameraDynamicsFunctionLibrary::OrientationAwareRotationComposition
	 * for making changes to delta rot.
	 */
	virtual bool ProcessViewRotationBlended(AActor* ViewTarget, float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot);
	
	/**
	 * Blueprint level blended view rotation processing.
	 * This should be used for anything that will affect the actual view rotation of the camera.
	 * Blending between modifiers that process view rotation can cause unwanted roll in the camera, so should be done
	 * with caution.
	 * 
	* @param BlendAlpha - The alpha value for the blend. Keep in bind that blends are handled in the camera manager,
	 * so this value should not need to be used for anything other than custom blending.
	 * @param ViewRotation - The incoming view rotation.
	 * @param DeltaRot - The incoming delta rotation.
	 * @param OutViewRotation - The outgoing view rotation. Generally speaking, this should not be different to the
	 * incoming view rotation unless other rotation changes (including player input) are either non-existent or being ignored.
	 * @param OutDeltaRot - The outgoing delta rotation, which will be the main value to change in this function unless
	 * totally overriding the view rotation.
	 * @return - Return true if this should be the last modifier to process view rotation. Will prevent any subsequent
	 * modifiers from processing view rotation.
	 */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Camera Dynamics")
	bool BlueprintProcessViewRotationBlended(float BlendAlpha, float DeltaTime, FRotator ViewRotation, FRotator DeltaRot,
	                                         FRotator& OutViewRotation, FRotator& OutDeltaRot);
	
	/** Adjust the value of a colour by an int offset */
	UFUNCTION(BlueprintPure, Category = "Camera Dynamics")
	static FColor AdjustColourValue(const int32 ValueOffset = -64, const FColor& InColour = FColor::White);
	
	/** Get the value of a runtime float curve at a given time. Wraps the function library version. */
	UFUNCTION(BlueprintPure, Category = "Camera Dynamics")
	static float GetRuntimeFloatCurveValue(const FRuntimeFloatCurve& Curve, const float InTime);
	
	/** Draw debug text on the canvas at a world location and optional offset */
	static void DrawDebugTextProjected(TObjectPtr<UCanvas> Canvas, const UFont* DrawFont, const FString& Text,
	                                   const FVector& WorldLocation, const FVector2D Offset = FVector2D::ZeroVector,
	                                   bool bDrawLineFromOffset = true, float FontScale = 1.0f);
	
	/** Remove this camera modifier from the camera modifier list */
	virtual void RemoveSelfFromModifierList();
	
	/**
	 * Get the alpha value for the current blend, based on the appropriate custom blend
	 * if there is a custom blend for the current blend type.
	 */
	UFUNCTION(BlueprintPure, Category = "Camera Dynamics")
	float GetCustomBlendAlpha(bool bBlendIn) const;
	
	/** Should this camera modifier draw debug during this frame */
	UFUNCTION(BlueprintPure, Category = "Camera Dynamics")
	bool IsDebugEnabled() const { return bDrawDebugInfoThisFrame; }

	virtual float GetTargetAlpha() override;

	UFUNCTION(BlueprintCallable, Category = "Camera Dynamics")
	void BlendToNewTargetAlpha(float NewTargetAlpha, float BlendTime);

	UFUNCTION(BlueprintCallable, Category = "Camera Dynamics")
	void ResetBlendState();
	
private:

	UFUNCTION()
	void OnViewTargetChangeStart(AActor* NewViewTarget, FViewTargetTransitionParams TransitionParams);

	float CustomTargetBlendAlpha;
	float CustomTargetBlendTime;
	
	virtual bool ModifyCamera(float DeltaTime, FMinimalViewInfo& InOutPOV) override;
	
	bool bDrawDebugInfoThisFrame;
	
	bool bMarkedForRemoval;
	FTimerHandle RemovalTimerHandle;

	// The alpha value that the modifier had before it was blended out due to a view target change with a matching tag
	// This is used to blend back in when the view target changes back to one with the matching tag
	float AlphaBeforeViewTargetTagBlendOut;
	
	#if WITH_EDITOR
	/*
	 * Copying of data asset values from the editor to the runtime modifier
	 *
	 * This needs to be done so that game code changes don't get written to the data asset during PIE.
	 * This stuff won't run in packaged builds, and doesn't need to since all code will be done on runtime modifiers.
	 */
protected:
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
private:
	FTimerHandle CopyPropertiesTimerHandle;
	void CopyPropertiesToRuntimeModifier();
	
	#endif

	
};
