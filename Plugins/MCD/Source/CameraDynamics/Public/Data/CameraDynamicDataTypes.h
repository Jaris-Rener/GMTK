// Copyright (c) 2024, Evelyn Schwab. All rights reserved.

#pragma once
#include "CoreMinimal.h"
#include "Curves/CurveFloat.h"
#include "CameraDynamicDataTypes.Generated.h"

UENUM(BlueprintType)
enum ECameraSourcePosition
{
	CDPOS_EyeHeight				UMETA(DisplayName = "Eye Height"),
	CDPOS_Socket				UMETA(DisplayName = "Socket"),
	CDPOS_LocalPosition			UMETA(DisplayName = "Local Position"),
	CDPOS_AbsPosition			UMETA(DisplayName = "Absolute Position")
};

/**
 * Struct for the source position for a camera.
 * Can be used for anything that requires a position sourced from one of the ECameraSourcePosition inputs,
 * mainly the base position of a camera but can also be used for things like the start point of a trace.
 */
USTRUCT(BlueprintType)
struct FCameraSourcePositionData
{
    GENERATED_BODY()

public:
	
    /** The type of source position to be used */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera Dynamics")
	TEnumAsByte<ECameraSourcePosition> CameraSourcePosition;
	
    /** The name of the socket to be used when the source position is Socket */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera Dynamics",
		meta = (EditCondition = "CameraSourcePosition == ECameraSourcePosition::CDPOS_Socket", EditConditionHides))
	FName SocketName;

	/** The vector to use when the source position is Local Position */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera Dynamics",
	meta = (EditCondition = "CameraSourcePosition == ECameraSourcePosition::CDPOS_LocalPosition", EditConditionHides))
	FVector LocalPosition;

	/** The vector to use when the source position is Absolute Position */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera Dynamics",
	meta = (EditCondition = "CameraSourcePosition == ECameraSourcePosition::CDPOS_AbsPosition", EditConditionHides))
	FVector AbsolutePosition;
	
	FCameraSourcePositionData()
    {
	    CameraSourcePosition = CDPOS_EyeHeight;
		SocketName = TEXT("head");
		LocalPosition = FVector::ZeroVector;
		AbsolutePosition = FVector::ZeroVector;
    }
	
    /**
     * Find the source position for a camera based on the source position data.
     * @param Pawn - The pawn to use in finding the source position, if the source position requires a pawn
     */
    FVector FindSourcePosition(const TObjectPtr<APawn> Pawn) const;
};

/**
 * Struct for offsets on a location.
 * Mainly used for adjusting the local or global position of a camera.
 */
USTRUCT(BlueprintType)
struct FCameraOffsetPositionData
{
	GENERATED_BODY()

public:

	/** Offset of the camera in world space from the point before this modifier is active.
	 * You probably don't want to use this unless you have a fixed camera. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Dynamics", AdvancedDisplay)
	FVector TargetOffset;

	/** Offset of the camera in local space. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Dynamics", AdvancedDisplay)
	FVector SocketOffset;
	
	FCameraOffsetPositionData()
	{
		TargetOffset = FVector::ZeroVector;
        SocketOffset = FVector::ZeroVector;
	}

	FVector GetOffsetPosition(const FVector& SourcePosition, const FRotator& SourceRotation) const;
	
};

/**
 * Struct to scale and interpret a vector based on the XY and Z axes. 
 */
USTRUCT(BlueprintType, Category = "Camera Dynamics")
struct CAMERADYNAMICS_API FCDCameraAxisData
{
	GENERATED_BODY()
	
	FCDCameraAxisData()
	{
		bXYActive = true;
		XYScale = 1.0f;
		bZActive = true;
		ZScale = 1.0f;
	}

	/** Should we use the XY axis of the vector */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Dynamics")
	bool bXYActive;

	/** The scale of the XY axis */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Dynamics", DisplayName = "Influence XY",
		meta = (EditCondition = "bXYActive", UIMin = 0.0f, UIMax = 1.0f))
	float XYScale;

	/** Should we use the Z axis of the vector */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Dynamics")
	bool bZActive;

	/** The scale of the Z axis */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Dynamics", DisplayName = "Influence Z",
		meta = (EditCondition = "bZActive", UIMin = 0.0f, UIMax = 1.0f))
	float ZScale;

	float GetXYInfluence() const;
	float GetZInfluence() const;
	FVector ProcessAxis(const FVector& A, const FVector& B) const;
};


/**
 * Enum representing the way in which a curve should be applied to a camera parameter.
 */
UENUM(BlueprintType)
enum ECameraCurveModType
{
	ECM_Additive			UMETA(DisplayName = "Additive"),
	ECM_Multiplicative		UMETA(DisplayName = "Multiplicative"),
	ECM_Absolute			UMETA(DisplayName = "Absolute")
};


/**
 * Struct for a curve that can be applied to a camera parameter in a specific way.
 * Does not apply the curve in any way, as that is per implementation. 
 */
USTRUCT(BlueprintType)
struct FCameraCurveModData
{
	GENERATED_BODY()
public:
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera Dynamics")
	FRuntimeFloatCurve Curve;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera Dynamics")
	TEnumAsByte<ECameraCurveModType> CurveEvaluationType;

	FCameraCurveModData()
	{
		CurveEvaluationType = ECM_Additive;
	}
};

