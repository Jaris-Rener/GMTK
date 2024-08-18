// Copyright (c) 2024, Evelyn Schwab. All rights reserved.

#include "Data/CameraDynamicDataTypes.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"

/*
 * Camera source position and offsets
 */

FVector FCameraSourcePositionData::FindSourcePosition(const TObjectPtr<APawn> Pawn) const
{
	FVector SourcePosition = FVector::ZeroVector;
	switch (CameraSourcePosition)
	{
	case CDPOS_EyeHeight:
		if (!IsValid(Pawn)) { break; }
		SourcePosition = Pawn->GetActorLocation() + FVector(0.0f, 0.0f, Pawn->BaseEyeHeight);
		break;
		
	case CDPOS_Socket:
		if (const ACharacter* OwnerControllerCharacter = Cast<ACharacter>(Pawn))
		{
			SourcePosition = OwnerControllerCharacter->GetMesh()->GetSocketLocation(SocketName);
		}
		break;
		
	case CDPOS_LocalPosition:
		SourcePosition = LocalPosition + Pawn->GetActorLocation();
		break;
		
	case CDPOS_AbsPosition:
		SourcePosition = AbsolutePosition;
		break;
		
	default:
		break;
	}

	return SourcePosition;
}

FVector FCameraOffsetPositionData::GetOffsetPosition(const FVector& SourcePosition, const FRotator& SourceRotation) const
{
	FVector NewViewLocation = SourcePosition;
	
	// Offset the position of the start location
	// Get the base offset in world space
	NewViewLocation += TargetOffset;
	
	// Offset the camera by the socket offset, rotated by the target rotation
	NewViewLocation += FRotationMatrix(SourceRotation).TransformVector(SocketOffset);

	return NewViewLocation;
}

/*
 * Camera axis data
 */

float FCDCameraAxisData::GetXYInfluence() const
{
	if (bXYActive) return XYScale;
	return 0.0f;
}

float FCDCameraAxisData::GetZInfluence() const
{
	if (bZActive) return ZScale;
	return 0.0f;
}

FVector FCDCameraAxisData::ProcessAxis(const FVector& A, const FVector& B) const
{
	FVector NewVector;
	NewVector.X = FMath::Lerp(A.X, B.X, GetXYInfluence());
	NewVector.Y = FMath::Lerp(A.Y, B.Y, GetXYInfluence());
	NewVector.Z = FMath::Lerp(A.Z, B.Z, GetZInfluence());
	return NewVector;
}
