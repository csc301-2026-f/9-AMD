// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "Points/Blueprint/MultiBinaryPointBlueprintLibrary.h"
#include "Points/MultiBinaryPoint.h"
#include "Common/BlueprintErrorUtils.h"

TInstancedStruct<FMultiBinaryPoint> UMultiBinaryPointBlueprintLibrary::ArrayToMultiBinaryPoint(const TArray<bool>& InValues)
{
    return TInstancedStruct<FMultiBinaryPoint>::Make<FMultiBinaryPoint>(InValues);
}

TArray<bool> UMultiBinaryPointBlueprintLibrary::MultiBinaryPointToArray(const TInstancedStruct<FMultiBinaryPoint>& InMultiBinaryPoint)
{
    // Type check: ensure the InstancedStruct is actually a FMultiBinaryPoint
    if (!InMultiBinaryPoint.IsValid())
    {
        RaiseInvalidInstancedStructError(TEXT("MultiBinaryPointToArray"));
        return TArray<bool>();
    }

    const FMultiBinaryPoint* TypedPoint = InMultiBinaryPoint.GetPtr<FMultiBinaryPoint>(); // Ensure the struct is initialized

    if (!TypedPoint)
    {
        RaiseInstancedStructTypeMismatchError(InMultiBinaryPoint, TEXT("FMultiBinaryPoint"), TEXT("MultiBinaryPointToArray"));
        return TArray<bool>();
    }
    return TypedPoint->Values;
}


