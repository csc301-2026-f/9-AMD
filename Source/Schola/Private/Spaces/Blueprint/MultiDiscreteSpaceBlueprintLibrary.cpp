// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "Spaces/Blueprint/MultiDiscreteSpaceBlueprintLibrary.h"
#include "Spaces/MultiDiscreteSpace.h"
#include "Common/BlueprintErrorUtils.h"

TInstancedStruct<FMultiDiscreteSpace> UMultiDiscreteSpaceBlueprintLibrary::ArrayToMultiDiscreteSpace(const TArray<int32>& InHigh)
{
    if (InHigh.Num() > 0)
    {
        return TInstancedStruct<FMultiDiscreteSpace>::Make(InHigh);
    }
    return TInstancedStruct<FMultiDiscreteSpace>::Make();
}

TArray<int32> UMultiDiscreteSpaceBlueprintLibrary::MultiDiscreteSpaceToArray(const TInstancedStruct<FMultiDiscreteSpace>& InMultiDiscreteSpace)
{
    // Type check: ensure the InstancedStruct is actually a FMultiDiscreteSpace
    if (!InMultiDiscreteSpace.IsValid())
    {
        RaiseInvalidInstancedStructError(TEXT("MultiDiscreteSpaceToArray"));
        return TArray<int32>();
    }

    const FMultiDiscreteSpace* TypedSpace = InMultiDiscreteSpace.GetPtr<FMultiDiscreteSpace>();
    
    if (!TypedSpace)
    {
        RaiseInstancedStructTypeMismatchError(InMultiDiscreteSpace, TEXT("FMultiDiscreteSpace"), TEXT("MultiDiscreteSpaceToArray"));
        return TArray<int32>();
    }

    return TypedSpace->High;
}
