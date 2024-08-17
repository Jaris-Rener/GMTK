// Copyright (c) 2024, Evelyn Schwab. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Curves/CurveVector.h"
#include "Data/CameraDynamicDataTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CameraDynamicsFunctionLibrary.generated.h"

struct FRuntimeFloatCurve;
class ACDPlayerCameraManager;
/**
 * 
 */
UCLASS()
class CAMERADYNAMICS_API UCameraDynamicsFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
	/**
	 * Get the camera dynamics camera manager from a player controller. Returns nullptr if the manager is not a camera dynamics manager.
	 * @param Controller - The player controller that owns the camera dynamics camera manager
	 * @return - The player camera manger, will not be valid if the camera manager is not a camera dynamics manager
	 */
	UFUNCTION(BlueprintPure, Category = "Camera Dynamics", meta = (DefaultToSelf = "Controller"))
	static ACDPlayerCameraManager* GetCameraDynamicsCameraManager(const APlayerController* Controller);
	
	/**
	 * Get the Y of a runtime float curve at time X. More straightforward than the other ways of getting a runtime float results.
	 * More useful for C++ implementations since there is already a good BP node for this.
	 * @param Curve - The runtime float curve to evaluate
	 * @param Time - The time to evaluate the curve at
	 * @return - THe value of the curve at the given time
	 */
	UFUNCTION(BlueprintPure, Category = "Camera Dynamics")
	static float EvaluateRuntimeFloatCurve(const FRuntimeFloatCurve& Curve, const float& Time);

	/**
	 * Get the Y of a runtime vector curve at time X. More straightforward than the other ways of getting a runtime vector results.
	 * More useful for C++ implementations since there is already a good BP node for this.
	 * @param Curve - The runtime float curve to evaluate
	 * @param Time - The time to evaluate the curve at
	 * @return - THe value of the curve at the given time
	 */
	UFUNCTION(BlueprintPure, Category = "Camera Dynamics")
	static FVector EvaluateRuntimeVectorCurve(const FRuntimeVectorCurve& Curve, const float& Time);
	
	/**
	 * Scale a vector by camera axis data.
	 * Camera axis data is a struct that contains information about how to interpret a vector in the XY and Z axes.
	 * @param InVector - The vector to process
	 * @param AxisData - The relevant camera axis data
	 * @return - The processed vector
	 */
	UFUNCTION(BlueprintPure, Category = "Camera Dynamics")
	static FVector ScaleAxis(const FVector& InVector, const FCDCameraAxisData& AxisData);
	
	/**
	 * Blend two vectors by axis data.
	 * Camera axis data is a struct that contains information about how to interpret a vector in the XY and Z axes.
	 */
	UFUNCTION(BlueprintPure, Category = "Camera Dynamics")
	static FVector BlendVectorsByAxisData(const FVector& A, const FVector& B, const FCDCameraAxisData& AxisData);
	
	/**
	 * Get the offset position of a camera from a position data struct.
	 * This is a blueprint wrapper for the function, C++ implementations are better off using FCameraOffsetPositionData::GetOffsetPosition.
	 */
	UFUNCTION(BlueprintPure, Category = "Camera Dynamics")
	static FVector GetOffsetPosition(const FCameraOffsetPositionData& PositionData,
	                                 const FVector& CurrentPosition = FVector::ZeroVector,
	                                 const FRotator& CurrentRotation = FRotator::ZeroRotator);
	
	/**
	 * Interpolate a float, returning input A at MinInterpSpeed and input B at MaxInterpSpeed (unless they are negative).
	 * The point of this function is to ensure that the A and B values are returned immediately if the interp speed to too high or too low.
	 * This is designed to be used for camera dynamics, but can be used for any interpolation.
	 */
	UFUNCTION(BlueprintPure, Category = "Camera Dynamics")
	static float CameraFInterp(const float& A, const float& B, const float& DeltaTime, const float& InterpSpeed = 1.0f, bool bConstantStep = false,
	                           float MinInterpSpeed = -1.0f, float MaxInterpSpeed = -1.0f);

	/**
	 * Interpolate a vector, returning input A at MinInterpSpeed and input B at MaxInterpSpeed (unless they are negative).
	 * The point of this function is to ensure that the A and B values are returned immediately if the interp speed to too high or too low.
	 * This is designed to be used for camera dynamics, but can be used for any interpolation.
	 */
	UFUNCTION(BlueprintPure, Category = "Camera Dynamics")
	static FVector CameraVInterp(FVector A, FVector B, float DeltaTime, float InterpSpeed = 1.0f, bool bConstantStep = false,
	                             float MinInterpSpeed = -1.0f, float MaxInterpSpeed = -1.0f);

	/**
	 * Compose two transforms while accounting for a reference quaternion and an orientation vector to ensure
	 * that the resulting transform avoids unnecessary roll.
	 * This is particularly useful in scenarios where maintaining a specific alignment or orientation,
	 * such as with gravity or a camera view, is crucial.
	 * 
	 * (This is really just a wrapper for UCameraDynamicsFunctionLibrary::OrientationAwareComposeRotations),
	 * but it also handles the translation and scale components of the transform. If you want to do the rotation part,
	 * just use that.
	 * 
	 * @param Child - The local transform to be applied on top of the parent
	 * @param Parent - The base transform to which the child transform will be applied
	 * @param ReferenceQuat - Reference quaternion referencing a baseline orientation. Likely an identity quat,
	 * or a camera quat from a previous frame.
	 * @param Orientation - A vector representing the desired up direction or alignment axis.
	 * Typically, this is the world's -up vector or the direction of gravity.
	 */
	UFUNCTION(BlueprintPure, Category = "Camera Dynamics")
	static FTransform OrientationAwareComposeTransforms(const FTransform& Child, const FTransform& Parent,
	                                                    const FQuat& ReferenceQuat,
	                                                    const FVector& Orientation);

	/**
	 * Compose two rotations while accounting for a reference quaternion and an orientation vector to ensure
	 * that the resulting transform avoids unnecessary roll.
	 * This is particularly useful in scenarios where maintaining a specific alignment or orientation,
	 * such as with gravity or a camera view, is crucial.
	 *
	 * Thanks to Hristo Enchev for the original implementation of this function.
	 * 
	 * @param Child - The local quat to be applied on top of the parent
	 * @param Parent - The base quat to which the child transform will be applied
	 * @param ReferenceQuat - Reference quaternion referencing a baseline orientation. Likely an identity quat,
	 * or a camera quat from a previous frame.
	 * @param Orientation - A vector representing the desired up direction or alignment axis.
	 * Typically, this is the world's -up vector or the direction of gravity.
	 */
	UFUNCTION(BlueprintPure, Category = "Camera Dynamics")
	static FQuat OrientationAwareComposeRotations(const FQuat& Child, const FQuat& Parent,
														const FQuat& ReferenceQuat,
														const FVector& Orientation);
};
