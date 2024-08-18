// Copyright (c) 2024, Evelyn Schwab. All rights reserved.


#include "Modifiers/CDCameraModifier_Position_DynamicZ.h"

#include "CameraDynamicsFunctionLibrary.h"
#include "DrawDebugHelpers.h"
#include "Camera/PlayerCameraManager.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"

UCDCameraModifier_Position_DynamicZ::UCDCameraModifier_Position_DynamicZ()
{
	SnapThreshold = 10.0f;
	
	bShouldDirectInterpZ = false;
	DebugColour = FColor::Purple;
	FriendlyName = FText::FromString(TEXT("Dynamic Z Position"));
}


void UCDCameraModifier_Position_DynamicZ::AddedToCamera(APlayerCameraManager* Camera)
{
	Super::AddedToCamera(Camera);

	// This is not the exact camera position, and might cause issues, but should be good enough most of the time
	LastGroundedPosition = Camera->GetCameraLocation();

	// Bind LandedDelegate to OnCharacterLanded to set up immediate Z interpolation
	if (!IsValid(Camera->GetOwningPlayerController())) return;
	if (!Camera->GetOwningPlayerController()->OnPossessedPawnChanged.IsAlreadyBound(this, &ThisClass::OnOwnerPossessedPawnChanged))
	{
		Camera->GetOwningPlayerController()->OnPossessedPawnChanged.AddDynamic(this, &ThisClass::OnOwnerPossessedPawnChanged);
	}
	// Run OnPossessedPawnChanged to set up the delegate, since the owner should already be possessed at this point
	if (!IsValid(GetOwnerControlledPawn())) return;
	OnOwnerPossessedPawnChanged(nullptr, GetOwnerControlledPawn());
}

void UCDCameraModifier_Position_DynamicZ::OnOwnerPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	if (!IsValid(NewPawn)) return;
	ACharacter* C = Cast<ACharacter>(NewPawn);
	if (!C->LandedDelegate.IsAlreadyBound(this, &UCDCameraModifier_Position_DynamicZ::OnCharacterLanded))
	{
		C->LandedDelegate.AddDynamic(this, &UCDCameraModifier_Position_DynamicZ::OnCharacterLanded);
	}
}

void UCDCameraModifier_Position_DynamicZ::OnCharacterLanded(const FHitResult& Hit)
{
	bShouldDirectInterpZ = true;
}

void UCDCameraModifier_Position_DynamicZ::ModifyCameraBlended(float DeltaTime, FVector ViewLocation,
	FRotator ViewRotation, float FOV, FVector& NewViewLocation, FRotator& NewViewRotation, float& NewFOV)
{
	Super::ModifyCameraBlended(DeltaTime, ViewLocation, ViewRotation, FOV, NewViewLocation, NewViewRotation, NewFOV);

	TargetPosition = NewViewLocation;
	
	UCharacterMovementComponent* CharacterMovement;
	if (!IsValid(GetOwnerControlledCharacter())) return;
	if (!IsValid(CharacterMovement = GetOwnerControlledCharacter()->GetCharacterMovement())) return;
	
	// If we are grounded
	if (CharacterMovement->IsMovingOnGround())
	{
		// Check if the camera is already at target
		if (bShouldDirectInterpZ)
		{
			if (FMath::IsNearlyEqual(CurrentPosition.Z, NewViewLocation.Z,0.1f))
			{
				bShouldDirectInterpZ = false;
			}
			else
			{
				const float DirectInterp = GetRuntimeFloatCurveValue(ReturnSpeed, FMath::Abs(CurrentPosition.Z - NewViewLocation.Z));
				NewViewLocation.Z = UCameraDynamicsFunctionLibrary::CameraFInterp(
					CurrentPosition.Z, NewViewLocation.Z, DeltaTime, DirectInterp, false, -1.0f, SnapThreshold);
			}
		}
		CurrentPosition = NewViewLocation;
		LastGroundedPosition = NewViewLocation;
		return;
	}
	
	// Evaluate the interp speed if we're not grounded
	LastGroundedPosition.X = NewViewLocation.X;
	LastGroundedPosition.Y = NewViewLocation.Y;
	
	const float DistanceFromLastGrounded = NewViewLocation.Z - LastGroundedPosition.Z;
	const float ZInterp = GetRuntimeFloatCurveValue(AirborneInterpSpeed, DistanceFromLastGrounded);
	NewViewLocation.Z = UCameraDynamicsFunctionLibrary::CameraFInterp(CurrentPosition.Z, NewViewLocation.Z, DeltaTime, ZInterp, false,
												  0.0f, SnapThreshold);
	
	CurrentPosition = NewViewLocation;
}

void UCDCameraModifier_Position_DynamicZ::RemoveSelfFromModifierList()
{
	Super::RemoveSelfFromModifierList();

	// Clear bound dynamic delegates
	ACharacter* C = GetOwnerControlledCharacter();
	if (!IsValid(C)) return;
	if (!C->LandedDelegate.IsAlreadyBound(this, &UCDCameraModifier_Position_DynamicZ::OnCharacterLanded))
	{
		C->LandedDelegate.RemoveDynamic(this, &UCDCameraModifier_Position_DynamicZ::OnCharacterLanded);
	}
	
	APlayerController* PC = GetOwnerController();
	if (!PC->OnPossessedPawnChanged.IsAlreadyBound(this, &ThisClass::OnOwnerPossessedPawnChanged))
	{
		PC->OnPossessedPawnChanged.RemoveDynamic(this, &ThisClass::OnOwnerPossessedPawnChanged);
	}
}

void UCDCameraModifier_Position_DynamicZ::DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay,
	float& YL, float& YPos)
{
	Super::DisplayDebug(Canvas, DebugDisplay, YL, YPos);

	const UFont* DrawFont = GEngine->GetSmallFont();
	int LineNumber = FMath::CeilToInt(YPos / YL);
	Canvas->SetDrawColor(DebugColour);
	
	if (!CurrentPosition.Equals(TargetPosition, 0.01f))
	{
		DrawDebugCanvasWireSphere(Canvas, CurrentPosition, AdjustColourValue(-64, DebugColour), 10.0f, 16);
        DrawDebugCanvasLine(Canvas, CurrentPosition, TargetPosition, AdjustColourValue(-64, DebugColour));
		DrawDebugTextProjected(Canvas, DrawFont, FString::Printf(TEXT("%i : Dynamic Z position"), Priority), CurrentPosition, DebugTextBaseOffset);
        
        if (!CurrentPosition.Equals(LastGroundedPosition, 0.01f))
        {
        	DrawDebugCanvasWireSphere(Canvas, LastGroundedPosition, AdjustColourValue(-128, DebugColour), 10.0f, 16);
        }
	}
	
	YPos = LineNumber * YL;
}
