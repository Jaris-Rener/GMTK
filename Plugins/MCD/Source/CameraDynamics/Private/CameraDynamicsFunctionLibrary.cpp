// Copyright (c) 2024, Evelyn Schwab. All rights reserved.


#include "CameraDynamicsFunctionLibrary.h"
#include "CDPlayerCameraManager.h"
#include "Curves/CurveFloat.h"
#include "GameFramework/PlayerController.h"

ACDPlayerCameraManager* UCameraDynamicsFunctionLibrary::GetCameraDynamicsCameraManager(
	const APlayerController* Controller)
{
	return Cast<ACDPlayerCameraManager>(Controller->PlayerCameraManager);
}

float UCameraDynamicsFunctionLibrary::EvaluateRuntimeFloatCurve(const FRuntimeFloatCurve& Curve, const float& Time)
{
	const FRichCurve* RichCurve = Curve.GetRichCurveConst();
	if (!RichCurve) return 0.0f;	// early return if the curve is not valid
	return RichCurve->Eval(Time);
}

FVector UCameraDynamicsFunctionLibrary::EvaluateRuntimeVectorCurve(const FRuntimeVectorCurve& Curve, const float& Time)
{
	FVector OutVector;

	// Loop through the vector values and evaluate the curve for each
	for (int32 i = 0; i < 3; i++)
	{
		const FRichCurve* RichCurve = Curve.GetRichCurveConst(i);
		if (!RichCurve) continue;	// early continue if the curve is not valid for this channel
		OutVector[i] = RichCurve->Eval(Time);
	}
	
	return OutVector;
}

FVector UCameraDynamicsFunctionLibrary::ScaleAxis(const FVector& InVector, const FCDCameraAxisData& AxisData)
{
	return AxisData.ProcessAxis(FVector::ZeroVector, InVector);
}

FVector UCameraDynamicsFunctionLibrary::BlendVectorsByAxisData(const FVector& A, const FVector& B,
	const FCDCameraAxisData& AxisData)
{
	return AxisData.ProcessAxis(A, B);
}

FVector UCameraDynamicsFunctionLibrary::GetOffsetPosition(const FCameraOffsetPositionData& PositionData,
                                                          const FVector& CurrentPosition, const FRotator& CurrentRotation)
{
	return PositionData.GetOffsetPosition(CurrentPosition, CurrentRotation);
}

float UCameraDynamicsFunctionLibrary::CameraFInterp(const float& A, const float& B, const float& DeltaTime, const float& InterpSpeed,
                                                    const bool bConstantStep, const float MinInterpSpeed, const float MaxInterpSpeed)
{
	// If the min or max interp speed are used, return A or B based if the interp speed is less than or
	// greater than the min or max interp speed
	if (InterpSpeed <= MinInterpSpeed && MinInterpSpeed >= 0.0f) return A;
	if (InterpSpeed >= MaxInterpSpeed && MaxInterpSpeed >= 0.0f) return B;

	if (!bConstantStep)
	{
		 return FMath::FInterpTo(A, B, DeltaTime, InterpSpeed);
	}
	return FMath::FInterpConstantTo(A, B, DeltaTime, InterpSpeed);
}

FVector UCameraDynamicsFunctionLibrary::CameraVInterp(FVector A, FVector B, float DeltaTime, float InterpSpeed,
                                                      bool bConstantStep, float MinInterpSpeed, float MaxInterpSpeed)
{
	// If the min or max interp speed are used, return A or B based if the interp speed is less than or
	// greater than the min or max interp speed
	if (InterpSpeed <= MinInterpSpeed && MinInterpSpeed >= 0.0f) return A;
	if (InterpSpeed >= MaxInterpSpeed && MaxInterpSpeed >= 0.0f) return B;

	if (!bConstantStep)
	{
		return FMath::VInterpTo(A, B, DeltaTime, InterpSpeed);
	}
	return FMath::VInterpConstantTo(A, B, DeltaTime, InterpSpeed);
}

FTransform UCameraDynamicsFunctionLibrary::OrientationAwareComposeTransforms(const FTransform& Child, const FTransform& Parent,
	const FQuat& ReferenceQuat, const FVector& Orientation)
{
	FTransform MultipliedTransform;
	// the A transform is treated as local space addition, over B
	//Regular T*T no scale
	// If the child loc is nearly zero we just use the parent's loc
	if (Child.GetLocation().IsNearlyZero())
	{
		MultipliedTransform.SetLocation(Parent.GetLocation());
	}
	else
	{
		MultipliedTransform.SetLocation(Parent.TransformPositionNoScale(Child.GetLocation()));
	}
	
	// Use the rotation composition function for handling function to get the combined rotation
	FQuat CombinedRotation = OrientationAwareComposeRotations(
		Child.GetRotation(),
		Parent.GetRotation(),
		ReferenceQuat,
		Orientation
	);
	
	MultipliedTransform.SetScale3D(Parent.GetScale3D()); 	// Set the scale of the result to be the same as Parent's scale
	
	return MultipliedTransform;
}

FQuat UCameraDynamicsFunctionLibrary::OrientationAwareComposeRotations(const FQuat& Child, const FQuat& Parent,
	const FQuat& ReferenceQuat, const FVector& Orientation)
{
	FQuat CombinedRotation = Parent * Child;
	
	// Calculate the roll to remove based on the Orientation vector and Y-axis of the combined rotation
	const double RollToRemove = (180.0) / UE_DOUBLE_PI * FMath::Asin(CombinedRotation.GetAxisY() | Orientation);
	// Create a quaternion to remove the calculated roll
	const FQuat RollQuat = FQuat(CombinedRotation.GetAxisX(), FMath::DegreesToRadians(RollToRemove));
	const double AdditiveRoll = Child.Rotator().Roll;
	// Calculate the original roll by comparing Parent's rotation with the ReferenceQuat
	const double OriginalRoll = (ReferenceQuat.Inverse() * Parent).Rotator().Roll;
	// Create a quaternion to represent the additional roll to preserve
	const FQuat AdditiveRollQuat = FRotator(0.0, 0.0, AdditiveRoll - OriginalRoll).Quaternion();
	// Apply the roll removal quaternion to the combined rotation
	CombinedRotation = RollQuat * CombinedRotation;
	// Apply the additional roll quaternion to the combined rotation
	CombinedRotation = CombinedRotation * AdditiveRollQuat;

	return CombinedRotation;
}
