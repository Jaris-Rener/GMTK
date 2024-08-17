// Copyright (c) 2024, Evelyn Schwab. All rights reserved.


#include "Modifiers/CDCameraModifier_Instanced.h"
#include "CameraDynamics.h"
#include "CameraDynamicsFunctionLibrary.h"
#include "CDCameraStack.h"
#include "CDPlayerCameraManager.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"
#include "Camera/PlayerCameraManager.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"


UCDCameraModifierInstanced::UCDCameraModifierInstanced()
{
	bDrawDebugInfoThisFrame = false;
	AlphaInTime = 1.0f;
	AlphaOutTime = 1.0f;
	bMarkedForRemoval = false;
	CameraDataSource = nullptr;
	
	// Default this to true, might be used to disable debug drawing on specific instances
	bDebug = true;
	bUseCustomPriority = false;
	DebugColour = FColor::White;
	DebugTextBaseOffset = FVector2D(100.0f, 0.0f);
	DebugLevel = DL_Light;

	FriendlyName = FText::FromString(GetNameSafe(this));

	// A < 0 value for these is treated as inactive/unset
	CustomTargetBlendAlpha = -1.0f;
	CustomTargetBlendTime = -1.0f;
	AlphaBeforeViewTargetTagBlendOut = -1.0f;

	bBlendOutForViewTargetWithMatchingTag = true;
	ViewTargetBlendOutTag = FName(TEXT("SequencerActor"));
	bWatchForViewTargetChange = true;
}

void UCDCameraModifierInstanced::AddedToCamera(APlayerCameraManager* Camera)
{
	Super::AddedToCamera(Camera);
	BlueprintAddedToCamera(Camera); // Trigger the blueprint event
	Cast<ACDPlayerCameraManager>(Camera)->OnViewTargetChangeStart.AddDynamic(this, &UCDCameraModifierInstanced::OnViewTargetChangeStart);
	EnableModifier();
}

void UCDCameraModifierInstanced::EnableModifier()
{
	Super::EnableModifier();
	bMarkedForRemoval = false;
}

void UCDCameraModifierInstanced::ModifyCamera(float DeltaTime, FVector ViewLocation, FRotator ViewRotation, float FOV,
	FVector& NewViewLocation, FRotator& NewViewRotation, float& NewFOV)
{
	Super::ModifyCamera(DeltaTime, ViewLocation, ViewRotation, FOV, NewViewLocation, NewViewRotation, NewFOV);

	const float A = GetCustomBlendAlpha(!bPendingDisable); // Get the alpha for custom blends, if custom blends are enabled
	
	if (A == 0.0f) return;

	// Native camera modification
	ModifyCameraBlended(DeltaTime, ViewLocation, ViewRotation, FOV, NewViewLocation,
		NewViewRotation, NewFOV);
	// Blueprint camera modification
	BlueprintModifyCameraBlended(A, DeltaTime, NewViewLocation, NewViewRotation, FOV, NewViewLocation,
		NewViewRotation, NewFOV);
	
	// Early return if this modifier is fully active
	if (A == 1.0f) return;

	// Interpolate the new values with the current values based on the alpha of this modifier
	NewViewLocation = FMath::Lerp(ViewLocation, NewViewLocation, A);
	NewViewRotation = FQuat::Slerp(ViewRotation.Quaternion(), NewViewRotation.Quaternion(), A).Rotator();
	NewFOV = FMath::Lerp(FOV, NewFOV, A);
}

void UCDCameraModifierInstanced::ModifyCameraBlended(float DeltaTime, FVector ViewLocation, FRotator ViewRotation,
	float FOV, FVector& NewViewLocation, FRotator& NewViewRotation, float& NewFOV)
{
}

bool UCDCameraModifierInstanced::ProcessViewRotation(AActor* ViewTarget, float DeltaTime, FRotator& OutViewRotation,
	FRotator& OutDeltaRot)
{
	// Get the true alpha value from custom blends, if custom blends are enabled
	const float A = GetCustomBlendAlpha(!bPendingDisable);

	if (A == 0.0f) return false;
	
	FRotator PotentialViewRotation = OutViewRotation;
	FRotator PotentialDeltaRot = OutDeltaRot;

	// Get the native blended values
	bool bReturn = ProcessViewRotationBlended(ViewTarget, DeltaTime, PotentialViewRotation, PotentialDeltaRot);
	// Get the BP blended values
	const bool bBPReturn = BlueprintProcessViewRotationBlended(A, DeltaTime, PotentialViewRotation,
	                                                           PotentialViewRotation, PotentialViewRotation,
	                                                           PotentialDeltaRot);
	if (!bReturn && bBPReturn) bReturn = true; // Return true later if the BP function returned true

	// If the rotation values didn't change, don't change them. This is to prevent the rotation from snapping back.
	if (PotentialViewRotation == OutViewRotation && PotentialDeltaRot == OutDeltaRot)
	{
		return false;
	}
	
	if (A == 1.0f)
	{
		OutViewRotation = PotentialViewRotation;
		OutDeltaRot = PotentialDeltaRot;
		return bReturn;
	}
	
	// Interpolate the new values with the current values based on the alpha of this modifier
	OutViewRotation = FQuat::Slerp(OutViewRotation.Quaternion(), PotentialViewRotation.Quaternion(), A).Rotator();
	OutDeltaRot = FQuat::Slerp(OutDeltaRot.Quaternion(), PotentialDeltaRot.Quaternion(), A).Rotator();
	
	return bReturn;
}

bool UCDCameraModifierInstanced::ProcessViewRotationBlended(AActor* ViewTarget, float DeltaTime,
	FRotator& OutViewRotation, FRotator& OutDeltaRot)
{
	return false;
}

bool UCDCameraModifierInstanced::ModifyCamera(float DeltaTime, FMinimalViewInfo& InOutPOV)
{
	// This is set up to call the blueprint and cpp versions of modify camera that are
	// actually intended to be used, then disable debug so that debug drawing can also
	// be handled in the modify camera functions
	Super::ModifyCamera(DeltaTime, InOutPOV);

	// Reset bDrawDebugInfo for next frame
	bDrawDebugInfoThisFrame = false;

	// This seems odd, but the base camera modifier does this too with the comment "allow subsequent modifiers to update"
	return false;
}

void UCDCameraModifierInstanced::MarkForRemoval()
{
	DisableModifier(false); // Start the blend out process
	if (!IsValid(CameraOwner))
	{
		UE_LOG(LogCameraDynamics, Error, TEXT("Camera owner is not valid, cannot mark modifier %s for removal"),
		       *GetNameSafe(this));
		return;
	}

	// If the alpha is already 0.0f, remove the modifier immediately
	if (AlphaOutTime <= 0.0f) RemoveSelfFromModifierList();
	
	GetWorld()->GetTimerManager().SetTimer(RemovalTimerHandle, this,
	                                       &UCDCameraModifierInstanced::RemoveSelfFromModifierList, AlphaOutTime,
	                                       false);
}

// Remove the modifier from the camera owner if the owner is valid
void UCDCameraModifierInstanced::RemoveSelfFromModifierList()
{
	if (!IsValid(CameraOwner))
	{
		UE_LOG(LogCameraDynamics, Error, TEXT("Camera owner is not valid, cannot remove modifier %s"), *GetNameSafe(this));
		return;
	}
	UE_LOG(LogCameraDynamics, Log, TEXT("Removing modifier %s from camera manager %s"), *GetNameSafe(this),
	       *GetNameSafe(CameraOwner));
	CameraOwner->RemoveCameraModifier(this);
}

void UCDCameraModifierInstanced::BlendToNewTargetAlpha(float NewTargetAlpha, float BlendTime)
{
	if (bPendingDisable) return;	// Don't use custom blends if we're pending disable
	CustomTargetBlendAlpha = NewTargetAlpha;
	CustomTargetBlendTime = BlendTime;
}

void UCDCameraModifierInstanced::ResetBlendState()
{
	CustomTargetBlendAlpha = -1.0f;
	CustomTargetBlendTime = -1.0f;
}

void UCDCameraModifierInstanced::OnViewTargetChangeStart(AActor* NewViewTarget,
	FViewTargetTransitionParams TransitionParams)
{
	// If we're blending from a view target with a matching tag to a view target without a matching tag, blend the alpha back up
	if (AlphaBeforeViewTargetTagBlendOut >= 0.0f
		&& !NewViewTarget->ActorHasTag(ViewTargetBlendOutTag)
		&& AlphaBeforeViewTargetTagBlendOut >= 0.0f)
	{
		BlendToNewTargetAlpha(AlphaBeforeViewTargetTagBlendOut, TransitionParams.BlendTime);
		AlphaBeforeViewTargetTagBlendOut = -1.0;
		return;
	}
	// If the old view target doesn't have a matching tag and the new one does, blend the alpha down to zero
	if (NewViewTarget->ActorHasTag(ViewTargetBlendOutTag))
	{
		AlphaBeforeViewTargetTagBlendOut = Alpha;
		BlendToNewTargetAlpha(0.0f, TransitionParams.BlendTime);
	}
}

float UCDCameraModifierInstanced::GetTargetAlpha()
{
	// If this modifier is pending disabling, return 0.0f
	// if we have an alternate target alpha, use that as the new target alpha. Otherwise, return 1.
	if (bPendingDisable) return 0.0f;
	if (CustomTargetBlendAlpha >= 0.0f) return CustomTargetBlendAlpha;
	
	return 1.0f;
}


void UCDCameraModifierInstanced::UpdateAlpha(float DeltaTime)
{
	float const TargetAlpha = GetTargetAlpha();
	
	float BlendTime;
	// if we have a alternate target time, use that as the new blend time
	if (CustomTargetBlendTime >= 0.0f) BlendTime = CustomTargetBlendTime;
	else BlendTime = (TargetAlpha == 0.f) ? AlphaOutTime : AlphaInTime;
	
	// no blend time means no blending, just go directly to target alpha
	if (BlendTime <= 0.f)
	{
		Alpha = TargetAlpha;
	}
	else if (Alpha > TargetAlpha)
	{
		// interpolate downward to target, while protecting against overshooting
		Alpha = FMath::Max<float>(Alpha - DeltaTime / BlendTime, TargetAlpha);
	}
	else
	{
		// interpolate upward to target, while protecting against overshooting
		Alpha = FMath::Min<float>(Alpha + DeltaTime / BlendTime, TargetAlpha);
	}
}

float UCDCameraModifierInstanced::GetCustomBlendAlpha(bool bBlendIn) const
{
	// Early return alpha if it is fully blended
	if (Alpha == 0.0f || Alpha == 1.0f) return Alpha;
	// Return the alpha value from the appropriate custom blend curve
	if (bBlendIn && bUseCustomBlendIn) return GetRuntimeFloatCurveValue(CustomBlendIn, AlphaInTime);
	if (bUseCustomBlendOut) return GetRuntimeFloatCurveValue(CustomBlendOut, AlphaOutTime);
	return Alpha;		// If we're not using custom blending for the blend type, return the alpha value
}


void UCDCameraModifierInstanced::DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL,
                                              float& YPos)
{
	Canvas->SetDrawColor(FColor::Cyan);
	const UFont* DrawFont = GEngine->GetSmallFont();
	int LineNumber = FMath::CeilToInt(YPos / YL);

	FString DataSourceName = TEXT("Unknown");
	if (CameraDataSource.IsValid()) DataSourceName = CameraDataSource->GetName();
	
	Canvas->DrawText(
		DrawFont, FString::Printf(
			TEXT("Modifier_Instanced %s from data %s, Priority %i, Alpha:%f"), *GetNameSafe(this), *DataSourceName, Priority, Alpha), 1 * YL,
		(LineNumber++) * YL);

	if (bMarkedForRemoval)
	{
		Canvas->DrawText(DrawFont, FString::Printf(TEXT("Modifier marked for removal, waiting on alpha == 0.0f")), 1 * YL,(LineNumber++) * YL);
	}
	
	YPos = LineNumber * YL;

	// Set bDrawDebugInfo for next frame
	bDrawDebugInfoThisFrame = true;
}



APawn* UCDCameraModifierInstanced::GetOwnerControlledPawn() const
{
	if(const APlayerController* OwningController = GetOwnerController())
	{
		return OwningController->GetPawn();
	}
	return nullptr;
}

ACharacter* UCDCameraModifierInstanced::GetOwnerControlledCharacter() const
{
	if (APawn* OwnerPawn = GetOwnerControlledPawn())
	{
		return Cast<ACharacter>(OwnerPawn);
	}
	return nullptr;
}

APlayerController* UCDCameraModifierInstanced::GetOwnerController() const
{
	if (!IsValid(CameraOwner)) return nullptr;
	return CameraOwner->GetOwningPlayerController();
}

FColor UCDCameraModifierInstanced::AdjustColourValue(const int32 ValueOffset, const FColor& InColour)
{
	return FColor(
		FMath::Clamp(InColour.R + ValueOffset, 0, 255),
		FMath::Clamp(InColour.G + ValueOffset, 0, 255),
		FMath::Clamp(InColour.B + ValueOffset, 0, 255),
		InColour.A
	);
}

float UCDCameraModifierInstanced::GetRuntimeFloatCurveValue(const FRuntimeFloatCurve& Curve, const float InTime)
{
	return UCameraDynamicsFunctionLibrary::EvaluateRuntimeFloatCurve(Curve, InTime);
}

void UCDCameraModifierInstanced::DrawDebugTextProjected(TObjectPtr<UCanvas> Canvas, const UFont* DrawFont,
                                                        const FString& Text, const FVector& WorldLocation,
                                                        const FVector2D Offset, bool bDrawLineFromOffset,
                                                        const float FontScale)
{
	// Get the height of the font to offset the text
	const float TextYOffset = DrawFont->GetMaxCharHeight() / 2.0f;
	const FVector ProjectedLocation = Canvas->Project(WorldLocation);
	
	if (bDrawLineFromOffset)
	{
		DrawDebugCanvas2DLine(Canvas, ProjectedLocation, ProjectedLocation + FVector(Offset, 0.0f), Canvas->DrawColor);
	}
	
	FVector2D TextTrueOffset = Offset;
	if (Offset.X < 0.0f)
	{
		TextTrueOffset.X -= DrawFont->GetStringSize(*Text);
	}

	FFontRenderInfo FontRenderInfo;
	FontRenderInfo.bEnableShadow = true;
	Canvas->DrawText(DrawFont, Text, ProjectedLocation.X + TextTrueOffset.X, ProjectedLocation.Y + TextTrueOffset.Y - TextYOffset,
	                 FontScale, FontScale, FontRenderInfo);
}

#if WITH_EDITOR

void UCDCameraModifierInstanced::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);

	UE_LOG(LogTemp, Warning, TEXT("Chain"));
	CopyPropertiesToRuntimeModifier();
}

// We use this to copy the properties from the editor to the runtime modifier
void UCDCameraModifierInstanced::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	UE_LOG(LogTemp, Warning, TEXT("Standard"));
}


void UCDCameraModifierInstanced::CopyPropertiesToRuntimeModifier()
{
	if (!RuntimeModifier.IsValid()) return;

	UEngine::FCopyPropertiesForUnrelatedObjectsParams CopyOptions;
	CopyOptions.bSkipCompilerGeneratedDefaults = false;
	UEngine::CopyPropertiesForUnrelatedObjects(this, RuntimeModifier.Get(), CopyOptions);
}

#endif
