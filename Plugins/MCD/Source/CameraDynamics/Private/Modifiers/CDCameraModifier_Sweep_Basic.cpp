// Copyright (c) 2024, Evelyn Schwab. All rights reserved.


#include "Modifiers/CDCameraModifier_Sweep_Basic.h"
#include "CollisionQueryParams.h"
#include "DrawDebugHelpers.h"
#include "Runtime/Engine/Classes/Engine/HitResult.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"

UCDCameraModifier_Sweep_Basic::UCDCameraModifier_Sweep_Basic()
{
	DebugColour = FColor::Red;
	FriendlyName = FText::FromString(TEXT("Basic Collision Trace"));
}

void UCDCameraModifier_Sweep_Basic::ModifyCamera(float DeltaTime, FVector ViewLocation, FRotator ViewRotation,
                                                 float FOV, FVector& NewViewLocation, FRotator& NewViewRotation, float& NewFOV)
{
	Super::ModifyCamera(DeltaTime, ViewLocation, ViewRotation, FOV, NewViewLocation, NewViewRotation, NewFOV);

	// Get the trace start point
	TraceStart = CameraTraceData.TraceStartPoint.FindSourcePosition(GetOwnerControlledPawn());
	
	TraceEnd = NewViewLocation;
	TraceHit = NewViewLocation;	// This gets set here for checking if the trace hit anything in debug drawing
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(GetOwnerControlledPawn());

	// TODO :: Add additional trace types (object, profile)

	// Promote the trace hit location to the new view location if a hit occurred
	FHitResult HitResultFromPawn;
	if (GetWorld()->SweepSingleByChannel(HitResultFromPawn, TraceStart, TraceEnd, FQuat::Identity,
	                                                           CameraTraceData.TraceChannel, FCollisionShape::MakeSphere(CameraTraceData.TraceRadius),
	                                                           TraceParams))
	{
		TraceHit = HitResultFromPawn.Location;
	}

	NewViewLocation = TraceHit;
}

void UCDCameraModifier_Sweep_Basic::DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL,
	float& YPos)
{
	Super::DisplayDebug(Canvas, DebugDisplay, YL, YPos);

	const UFont* DrawFont = GEngine->GetSmallFont();
	int LineNumber = FMath::CeilToInt(YPos / YL);
	
	DrawDebugCanvasWireSphere(Canvas, TraceStart, FColor::Red, 10.0f, 16);
	Canvas->SetDrawColor(FColor::Red);
	Canvas->DrawText(DrawFont, FString::Printf(TEXT("Camera trace start: %s"), *TraceStart.ToCompactString()), 2 * YL, (LineNumber++) * YL);
	DrawDebugTextProjected(Canvas, DrawFont, FString::Printf(TEXT("%i : Trace start"), Priority), TraceStart, DebugTextBaseOffset);
	
	const FColor TraceColor = TraceEnd.Equals( TraceHit, 0.5f) ? FColor::Red : FColor::Green;
	DrawDebugCanvasLine(Canvas, TraceStart, TraceHit, FLinearColor::Red);
	Canvas->SetDrawColor(TraceColor);
	Canvas->DrawText(DrawFont, FString::Printf(TEXT("Camera trace end/hit location: %s"), *TraceEnd.ToCompactString()), 2 * YL, (LineNumber++) * YL);

	YPos = LineNumber * YL;
}
