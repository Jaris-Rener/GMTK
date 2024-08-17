// Copyright (c) 2024, Evelyn Schwab. All rights reserved.


#include "CDPlayerCameraManager.h"
#include "CameraDynamics.h"
#include "CameraDynamicsFunctionLibrary.h"
#include "CDCameraStack.h"
#include "IXRTrackingSystem.h"
#include "Engine/Engine.h"
#include "Modifiers/CDCameraModifier_Instanced.h"

DECLARE_CYCLE_STAT(TEXT("Camera ProcessViewRotation CameraDynamics"), STAT_Camera_ProcessViewRotation_CameraDynamics, STATGROUP_Game);

ACDPlayerCameraManager::ACDPlayerCameraManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bUseOrientationAwareRotationComposition = true;
}

void ACDPlayerCameraManager::InitializeFor(APlayerController* PC)
{
	Super::InitializeFor(PC);
	if (IsValid(DefaultCameraData)) AddCameraData(DefaultCameraData); // Add the default camera data, if there is any
}


void ACDPlayerCameraManager::AddCameraData(UCDCameraData* NewCameraData)
{
	if (!IsValid(NewCameraData)) return; // Early return if the camera data is invalid
	
	// Add the instanced camera modifiers to the camera manager
    if (NewCameraData->CameraModifiers.Num() > 0)
    {
    	int32 Index = 0;
    	const int32 InitialModCount = ModifierList.Num();
    	for (UCDCameraModifierInstanced* CameraModifier : NewCameraData->CameraModifiers)
    	{
    		if (CameraModifier)
    		{
    			// Duplicate the camera modifier and set the camera data source on the duplicate modifier
    			UCDCameraModifierInstanced* RuntimeModifier = DuplicateObject(CameraModifier, this);
    			RuntimeModifier->CameraDataSource = NewCameraData;
    			CameraModifier->RuntimeModifier = RuntimeModifier;

				if (!IsValid(RuntimeModifier))
				{
					UE_LOG(LogCameraDynamics, Error, TEXT("Failed to duplicate camera modifier %s"), *CameraModifier->GetName());
					continue;
				}
    			
    			
    		    if (!RuntimeModifier->bUseCustomPriority)
                {
                    // Set the camera's priority to be the same as the index in the array, plus the initial modifier count
                    RuntimeModifier->Priority = InitialModCount + Index;
                }
    			// This is using an internal function that bypasses the normal method of adding camera modifiers
    			// The normal method uses TSubClassOf, and we want the modifiers to be EditInline
    			AddCameraModifierToList(RuntimeModifier);
    		}
    		Index++;
    	}

    	// Promote the new camera data as the active camera data
    	CameraDataList.Add(NewCameraData);
    }
}

bool ACDPlayerCameraManager::RemoveCameraData(UCDCameraData* CameraData)
{
	if (!IsValid(CameraData)) return false; // Early return if the camera data is invalid
	
	if (!CameraDataList.Contains(CameraData)) return false; // Early return if the camera data isn't in the list
	
	for (UCameraModifier* Modifier : CameraData->CameraModifiers)
	{
		UCDCameraModifierInstanced* InstancedModifier = Cast<UCDCameraModifierInstanced>(Modifier);
		if (!IsValid(InstancedModifier)) continue; // Early continue if the modifier isn't instanced
		
		// If the modifier's runtime modifier is in the list of modifiers, mark it for removal
		if (ModifierList.Contains(InstancedModifier->RuntimeModifier))
		{
			// Mark this modifier for removal, which will blend it out then remove it from the manager
			InstancedModifier->RuntimeModifier->MarkForRemoval();
		}
	}
	// Remove the camera data from the list
	CameraDataList.RemoveSingle(CameraData);
	if (CameraDataList.Contains(CameraData))
	{
		UE_LOG(LogCameraDynamics, Log, TEXT("Failed to remove camera data %s"), *CameraData->GetName());
	}
	return true;
}

void ACDPlayerCameraManager::RemoveAllCameraData()
{
	// We work with a copy of the array here as we don't want to modify the array while iterating over it
	TArray<UCDCameraData*> CameraDataListCopy = CameraDataList;
	for (UCDCameraData* CameraData : CameraDataListCopy)
	{
		RemoveCameraData(CameraData);
	}
}

UCameraModifier* ACDPlayerCameraManager::GetActiveModifierOfClass(
	const TSubclassOf<UCameraModifier> ModifierClass, const FGameplayTagContainer& RequiredTags)
{
	for (UCameraModifier* Modifier : ModifierList)
	{
		if (Modifier->IsA(ModifierClass))
		{
			if (RequiredTags.IsEmpty()) return Modifier;

			const UCDCameraModifierInstanced* InstancedModifier = Cast<UCDCameraModifierInstanced>(Modifier);
			if(!IsValid(InstancedModifier)) continue;

			// If the modifier has matching gameplay tags, return that modifier
			if (InstancedModifier->ModifierGameplayTags.HasAll(RequiredTags)) return Modifier;
			
			/*for (FName Tag : RequiredTags)	// Check if the modifier has the required tags, if there are required tags
			{
				const UCDCameraModifierInstanced* InstancedModifier = Cast<UCDCameraModifierInstanced>(Modifier);
				if(!IsValid(InstancedModifier)) continue;
				if (InstancedModifier->ModifierTags.Contains(Tag)) return Modifier;
			}*/
		}
	}
	return nullptr;
}

TArray<UCameraModifier*> ACDPlayerCameraManager::GetAllActiveModifiersOfClass(
	const TSubclassOf<UCameraModifier>& ModifierClass, const FGameplayTagContainer& RequiredTags)
{
	TArray<UCameraModifier*> Modifiers;
	for (UCameraModifier* Modifier : ModifierList)
	{
		if (Modifier->IsA(ModifierClass))
		{
			if (RequiredTags.IsEmpty())		// If there are no required tags, just add the modifier
			{
				Modifiers.Add(Modifier);
				continue;
			}

			const UCDCameraModifierInstanced* InstancedModifier = Cast<UCDCameraModifierInstanced>(Modifier);
			if(!IsValid(InstancedModifier)) continue;

			if (InstancedModifier->ModifierGameplayTags.HasAll(RequiredTags)) Modifiers.Add(Modifier);
			
			/*for (FName Tag : RequiredTags)	// Check if the modifier has the required tags, if there are required tags
			{
				const UCDCameraModifierInstanced* InstancedModifier = Cast<UCDCameraModifierInstanced>(Modifier);
				if(!IsValid(InstancedModifier)) continue;
				if (InstancedModifier->ModifierTags.Contains(Tag)) Modifiers.Add(Modifier);
			}*/
		}
	}
	return Modifiers;
}

void ACDPlayerCameraManager::ProcessViewRotation(float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot)
{
	SCOPE_CYCLE_COUNTER(STAT_Camera_ProcessViewRotation_CameraDynamics);
	for( int32 ModifierIdx = 0; ModifierIdx < ModifierList.Num(); ModifierIdx++ )
	{
		if( ModifierList[ModifierIdx] != NULL && 
			!ModifierList[ModifierIdx]->IsDisabled() )
		{
			if( ModifierList[ModifierIdx]->ProcessViewRotation(ViewTarget.Target, DeltaTime, OutViewRotation, OutDeltaRot) )
			{
				break;
			}
		}
	}

	// Add Delta Rotation.
	// If we are using orientation aware rotation composition, we compose the rots with UCameraDynamicsFunctionLibrary::OrientationAwareComposeRotations
	if (bUseOrientationAwareRotationComposition)
	{
		OutViewRotation = UCameraDynamicsFunctionLibrary::OrientationAwareComposeRotations(
			OutDeltaRot.Quaternion(),
			OutViewRotation.Quaternion(),
			OutViewRotation.Quaternion(),
			-FVector::UpVector).
		Rotator();
	}
	else
	{
		OutViewRotation += OutDeltaRot;
	}
	OutDeltaRot = FRotator::ZeroRotator;

	const bool bIsHeadTrackingAllowed =
		GEngine->XRSystem.IsValid() &&
		(GetWorld() != nullptr ? GEngine->XRSystem->IsHeadTrackingAllowedForWorld(*GetWorld()) : GEngine->XRSystem->IsHeadTrackingAllowed());
	if(bIsHeadTrackingAllowed)
	{
		// With the HMD devices, we can't limit the view pitch, because it's bound to the player's head.  A simple normalization will suffice
		OutViewRotation.Normalize();
	}
	else
	{
		// Limit Player View Axes
		LimitViewPitch( OutViewRotation, ViewPitchMin, ViewPitchMax );
		LimitViewYaw( OutViewRotation, ViewYawMin, ViewYawMax );
		LimitViewRoll( OutViewRotation, ViewRollMin, ViewRollMax );
	}
}

void ACDPlayerCameraManager::SetViewTarget(AActor* NewViewTarget, FViewTargetTransitionParams TransitionParams)
{
	Super::SetViewTarget(NewViewTarget, TransitionParams);

	// Adds a delegate for when view targets are changed
	OnViewTargetChangeStart.Broadcast(NewViewTarget, TransitionParams);
}
