// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "Points/Blueprint/BoxPointBlueprintLibrary.h"
#include "Points/BoxPoint.h"
#include "Common/BlueprintErrorUtils.h"
#include "Common/LogSchola.h"

TInstancedStruct<FBoxPoint> UBoxPointBlueprintLibrary::ArrayToBoxPoint(const TArray<float>& InValues)
{
	return TInstancedStruct<FBoxPoint>::Make(InValues);
}

TInstancedStruct<FBoxPoint> UBoxPointBlueprintLibrary::ArrayToBoxPointShaped(const TArray<float>& InValues, const TArray<int>& InShape)
{
	return TInstancedStruct<FBoxPoint>::Make(InValues, InShape);
}

TArray<float> UBoxPointBlueprintLibrary::BoxPointToArray(const TInstancedStruct<FBoxPoint>& InBoxPoint)
{
    // Type check: ensure the InstancedStruct is actually a FBoxPoint
    if (!InBoxPoint.IsValid())
    {
        RaiseInvalidInstancedStructError(TEXT("BoxPointToArray"));
        return TArray<float>();
    }

    const FBoxPoint* TypedPoint = InBoxPoint.GetPtr<FBoxPoint>();
    
    if (!TypedPoint)
    {
        RaiseInstancedStructTypeMismatchError(InBoxPoint, TEXT("FBoxPoint"), TEXT("BoxPointToArray"));
        return TArray<float>();
    }

    return TypedPoint->Values;
}

TInstancedStruct<FBoxPoint> UBoxPointBlueprintLibrary::VectorToBoxPoint(const FVector& InVector)
{
    TArray<float> Values;
    Values.Add(InVector.X);
    Values.Add(InVector.Y);
    Values.Add(InVector.Z);
    return TInstancedStruct<FBoxPoint>::Make(Values);
}

FVector UBoxPointBlueprintLibrary::BoxPointToVector(const TInstancedStruct<FBoxPoint>& InBoxPoint)
{
    // Type check: ensure the InstancedStruct is actually a FBoxPoint
    if (!InBoxPoint.IsValid())
    {
        RaiseInvalidInstancedStructError(TEXT("BoxPointToVector"));
        return FVector::ZeroVector;
    }

    const FBoxPoint* TypedPoint = InBoxPoint.GetPtr<FBoxPoint>();
    
    if (!TypedPoint)
    {
        RaiseInstancedStructTypeMismatchError(InBoxPoint, TEXT("FBoxPoint"), TEXT("BoxPointToVector"));
        return FVector::ZeroVector;
    }

    // Verify we have exactly 3 values
    if (TypedPoint->Values.Num() != 3)
    {
        UE_LOGFMT(LogSchola, Error, "UBoxPointBlueprintLibrary::BoxPointToVector(): Expected BoxPoint with 3 values, but got {0} values", TypedPoint->Values.Num());
        return FVector::ZeroVector;
    }

    return FVector(TypedPoint->Values[0], TypedPoint->Values[1], TypedPoint->Values[2]);
}

TInstancedStruct<FBoxPoint> UBoxPointBlueprintLibrary::RotatorToBoxPoint(const FRotator& InRotator)
{
    TArray<float> Values;
    //This will be normalized to [-180,180]
    FRotator NormalizedRotator = InRotator.GetNormalized();
    Values.Add(NormalizedRotator.Pitch);
    Values.Add(NormalizedRotator.Yaw);
    Values.Add(NormalizedRotator.Roll);
    return TInstancedStruct<FBoxPoint>::Make(Values);
}

FRotator UBoxPointBlueprintLibrary::BoxPointToRotator(const TInstancedStruct<FBoxPoint>& InBoxPoint)
{
    // Type check: ensure the InstancedStruct is actually a FBoxPoint
    if (!InBoxPoint.IsValid())
    {
        RaiseInvalidInstancedStructError(TEXT("BoxPointToRotator"));
        return FRotator::ZeroRotator;
    }

    const FBoxPoint* TypedPoint = InBoxPoint.GetPtr<FBoxPoint>();
    
    if (!TypedPoint)
    {
        RaiseInstancedStructTypeMismatchError(InBoxPoint, TEXT("FBoxPoint"), TEXT("BoxPointToRotator"));
        return FRotator::ZeroRotator;
    }

    // Verify we have exactly 3 values
    if (TypedPoint->Values.Num() != 3)
    {
        UE_LOGFMT(LogSchola, Error, "UBoxPointBlueprintLibrary::BoxPointToRotator(): Expected BoxPoint with 3 values, but got {0} values", TypedPoint->Values.Num());
        return FRotator::ZeroRotator;
    }

    return FRotator(TypedPoint->Values[0], TypedPoint->Values[1], TypedPoint->Values[2]);
}

TInstancedStruct<FBoxPoint> UBoxPointBlueprintLibrary::TransformToBoxPoint(const FTransform& InTransform)
{
    TArray<float> Values;
    
    // Add Location (3 values)
    const FVector& Location = InTransform.GetLocation();
    Values.Add(Location.X);
    Values.Add(Location.Y);
    Values.Add(Location.Z);
    
    // Add Rotation (3 values)
    const FRotator Rotation = InTransform.Rotator();
    //This will be normalized to [-180,180]
    Values.Add(Rotation.Pitch);
    Values.Add(Rotation.Yaw);
    Values.Add(Rotation.Roll);
    
    // Add Scale (3 values)
    const FVector& Scale = InTransform.GetScale3D();
    Values.Add(Scale.X);
    Values.Add(Scale.Y);
    Values.Add(Scale.Z);
    
    return TInstancedStruct<FBoxPoint>::Make(Values);
}

FTransform UBoxPointBlueprintLibrary::BoxPointToTransform(const TInstancedStruct<FBoxPoint>& InBoxPoint)
{
    // Type check: ensure the InstancedStruct is actually a FBoxPoint
    if (!InBoxPoint.IsValid())
    {
        RaiseInvalidInstancedStructError(TEXT("BoxPointToTransform"));
        return FTransform::Identity;
    }

    const FBoxPoint* TypedPoint = InBoxPoint.GetPtr<FBoxPoint>();
    
    if (!TypedPoint)
    {
        RaiseInstancedStructTypeMismatchError(InBoxPoint, TEXT("FBoxPoint"), TEXT("BoxPointToTransform"));
        return FTransform::Identity;
    }

    // Verify we have exactly 9 values
    if (TypedPoint->Values.Num() != 9)
    {
        UE_LOGFMT(LogSchola, Error, "UBoxPointBlueprintLibrary::BoxPointToTransform(): Expected BoxPoint with 9 values, but got {0} values", TypedPoint->Values.Num());
        return FTransform::Identity;
    }

    // Extract Location (first 3 values)
    FVector Location(TypedPoint->Values[0], TypedPoint->Values[1], TypedPoint->Values[2]);
    
    // Extract Rotation (next 3 values)
    FRotator Rotation(TypedPoint->Values[3], TypedPoint->Values[4], TypedPoint->Values[5]);
    
    // Extract Scale (last 3 values)
    FVector Scale(TypedPoint->Values[6], TypedPoint->Values[7], TypedPoint->Values[8]);
    
    return FTransform(Rotation, Location, Scale);
}
