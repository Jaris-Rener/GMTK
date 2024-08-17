// Copyright (c) 2024, Evelyn Schwab. All rights reserved.


#include "Modifiers/CDCameraModifier_Position_Distance.h"

UCDCameraModifier_Position_Distance::UCDCameraModifier_Position_Distance()
{
	TargetDistance = -350.0f;
	Distance = TargetDistance;
	bSmoothDistanceChanges = false;
	ChangeSmoothing = 2.0f;
	FriendlyName = FText::FromString(TEXT("Forward Distance Offset"));
}

void UCDCameraModifier_Position_Distance::AddedToCamera(APlayerCameraManager* Camera)
{
	Super::AddedToCamera(Camera);
	
	Distance = TargetDistance;
}

void UCDCameraModifier_Position_Distance::ModifyCameraBlended(float DeltaTime, FVector ViewLocation,
                                                              FRotator ViewRotation, float FOV, FVector& NewViewLocation, FRotator& NewViewRotation, float& NewFOV)
{
	Super::ModifyCameraBlended(DeltaTime, ViewLocation, ViewRotation, FOV, NewViewLocation, NewViewRotation, NewFOV);

	// Get the distance
	if (bSmoothDistanceChanges)
	{
		Distance = FMath::FInterpTo(Distance, TargetDistance, DeltaTime, ChangeSmoothing);
	}
	else Distance = TargetDistance;
	
	// Offset the camera along the rotation vector by the target distance
	NewViewLocation = NewViewLocation + NewViewRotation.Vector() * Distance;
}
