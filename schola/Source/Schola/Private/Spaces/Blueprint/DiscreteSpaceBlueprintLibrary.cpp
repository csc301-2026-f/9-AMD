// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "Spaces/Blueprint/DiscreteSpaceBlueprintLibrary.h"
#include "Spaces/DiscreteSpace.h"
#include "Common/BlueprintErrorUtils.h"

TInstancedStruct<FDiscreteSpace> UDiscreteSpaceBlueprintLibrary::Int32ToDiscreteSpace(int32 InHigh)
{
	return TInstancedStruct<FDiscreteSpace>::Make(InHigh);
}

int32 UDiscreteSpaceBlueprintLibrary::DiscreteSpaceToInt32(const TInstancedStruct<FDiscreteSpace>& InDiscreteSpace)
{
    // Type check: ensure the InstancedStruct is actually a FDiscreteSpace
    if (!InDiscreteSpace.IsValid())
    {
        RaiseInvalidInstancedStructError(TEXT("DiscreteSpaceToInt32"));
        return 0;
    }

    const FDiscreteSpace* TypedSpace = InDiscreteSpace.GetPtr<FDiscreteSpace>();
    
    if (!TypedSpace)
    {
        RaiseInstancedStructTypeMismatchError(InDiscreteSpace, TEXT("FDiscreteSpace"), TEXT("DiscreteSpaceToInt32"));
        return 0;
    }

    return TypedSpace->High;
}
