// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "Spaces/Blueprint/MultiBinarySpaceBlueprintLibrary.h"
#include "Spaces/MultiBinarySpace.h"
#include "Common/BlueprintErrorUtils.h"

TInstancedStruct<FMultiBinarySpace> UMultiBinarySpaceBlueprintLibrary::ShapeToMultiBinarySpace(int32 InShape)
{
    if (InShape > 0)
    {
        return TInstancedStruct<FMultiBinarySpace>::Make(InShape);
    }
    return TInstancedStruct<FMultiBinarySpace>::Make();
}

int32 UMultiBinarySpaceBlueprintLibrary::MultiBinarySpace_GetShape(const TInstancedStruct<FMultiBinarySpace>& InMultiBinarySpace)
{
    // Type check: ensure the InstancedStruct is actually a FMultiBinarySpace
    if (!InMultiBinarySpace.IsValid())
    {
        RaiseInvalidInstancedStructError(TEXT("MultiBinarySpace_GetShape"));
        return 0;
    }

    const FMultiBinarySpace* TypedSpace = InMultiBinarySpace.GetPtr<FMultiBinarySpace>();
    
    if (!TypedSpace)
    {
        RaiseInstancedStructTypeMismatchError(InMultiBinarySpace, TEXT("FMultiBinarySpace"), TEXT("MultiBinarySpace_GetShape"));
        return 0;
    }

    return TypedSpace->Shape;
}
