// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "Spaces/Blueprint/DictSpaceBlueprintLibrary.h"
#include "Spaces/DictSpace.h"
#include "Common/BlueprintErrorUtils.h"
#include "Common/InstancedStructUtils.h"

TInstancedStruct<FDictSpace> UDictSpaceBlueprintLibrary::MapToDictSpace(const TMap<FString, FInstancedStruct>& InSpaces)
{
    TInstancedStruct<FDictSpace> OutputDictSpace;
	OutputDictSpace.InitializeAs<FDictSpace>();
	FDictSpace* DictSpacePtr = OutputDictSpace.GetMutablePtr();
   
    DictSpacePtr->Spaces = reinterpret_cast<const TMap<FString, TInstancedStruct<FSpace>>&>(InSpaces);
	return OutputDictSpace;
}

TMap<FString, FInstancedStruct> UDictSpaceBlueprintLibrary::DictSpaceToMap(const TInstancedStruct<FDictSpace>& InDictSpace)
{
    TMap<FString, FInstancedStruct> Result;
    
    // Type check: ensure the InstancedStruct is actually a FDictSpace
    if (!InDictSpace.IsValid())
    {
        RaiseInvalidInstancedStructError(TEXT("DictSpaceToMap"));
        return Result;
    }

    const FDictSpace* TypedSpace = InDictSpace.GetPtr<FDictSpace>();

    if (!TypedSpace)
    {
        RaiseInstancedStructTypeMismatchError(InDictSpace, TEXT("FDictSpace"), TEXT("DictSpaceToMap"));
        return Result;
    }

    return reinterpret_cast<const TMap<FString, FInstancedStruct>&>(TypedSpace->Spaces);
}

bool UDictSpaceBlueprintLibrary::DictSpace_Add(TInstancedStruct<FDictSpace>& InOutDictSpace, const FString& InKey, const FInstancedStruct& InValue)
{
    // Type check: ensure the InstancedStruct is actually a FDictSpace
    if (!InOutDictSpace.IsValid())
    {
        RaiseInvalidInstancedStructError(TEXT("DictSpace_Add"));
        return false;
    }

    FDictSpace* TypedSpace = InOutDictSpace.GetMutablePtr<FDictSpace>();
    
    if (!TypedSpace)
    {
        RaiseInstancedStructTypeMismatchError(InOutDictSpace, TEXT("FDictSpace"), TEXT("DictSpace_Add"));
        return false;
    }

    if (!InValue.GetScriptStruct())
    {
        RaiseInvalidInstancedStructError(TEXT("DictSpace_Add"));
        return false;
    }

    if (!InValue.GetScriptStruct()->IsChildOf(FSpace::StaticStruct()))
    {
        RaiseInstancedStructTypeMismatchError(InValue, TEXT("FSpace"), TEXT("DictSpace_Add"));
        return false;
    }

    TypedSpace->Spaces.Add(InKey, ToTypedInstancedStruct<FSpace>(InValue));
    return true;
}

bool UDictSpaceBlueprintLibrary::DictSpace_Find(const TInstancedStruct<FDictSpace>& InDictSpace, const FString& InKey, FInstancedStruct& OutValue)
{
    // Type check: ensure the InstancedStruct is actually a FDictSpace
    if (!InDictSpace.IsValid())
    {
        RaiseInvalidInstancedStructError(TEXT("DictSpace_Find"));
        return false;
    }

    const FDictSpace* TypedSpace = InDictSpace.GetPtr<FDictSpace>();
    
    if (!TypedSpace)
    {
        RaiseInstancedStructTypeMismatchError(InDictSpace, TEXT("FDictSpace"), TEXT("DictSpace_Find"));
        return false;
    }

    const TInstancedStruct<FSpace>* Found = TypedSpace->Spaces.Find(InKey);
    
    if (!Found)
    {
        return false;
    }

    OutValue = ToUntypedInstancedStruct(*Found);
    return true;
}

bool UDictSpaceBlueprintLibrary::DictSpace_Contains(const TInstancedStruct<FDictSpace>& InDictSpace, const FString& InKey)
{
    // Type check: ensure the InstancedStruct is actually a FDictSpace
    if (!InDictSpace.IsValid())
    {
        RaiseInvalidInstancedStructError(TEXT("DictSpace_Contains"));
        return false;
    }

    const FDictSpace* TypedSpace = InDictSpace.GetPtr<FDictSpace>();
    
    if (!TypedSpace)
    {
        RaiseInstancedStructTypeMismatchError(InDictSpace, TEXT("FDictSpace"), TEXT("DictSpace_Contains"));
        return false;
    }
    
    return TypedSpace->Spaces.Contains(InKey);
}

bool UDictSpaceBlueprintLibrary::DictSpace_Remove(TInstancedStruct<FDictSpace>& InOutDictSpace, const FString& InKey)
{
    // Type check: ensure the InstancedStruct is actually a FDictSpace
    if (!InOutDictSpace.IsValid())
    {
        RaiseInvalidInstancedStructError(TEXT("DictSpace_Remove"));
        return false;
    }

    FDictSpace* TypedSpace = InOutDictSpace.GetMutablePtr<FDictSpace>();
    
    if (!TypedSpace)
    {
        RaiseInstancedStructTypeMismatchError(InOutDictSpace, TEXT("FDictSpace"), TEXT("DictSpace_Remove"));
        return false;
    }

    return TypedSpace->Spaces.Remove(InKey) > 0;
}

int32 UDictSpaceBlueprintLibrary::DictSpace_Length(const TInstancedStruct<FDictSpace>& InDictSpace)
{
    // Type check: ensure the InstancedStruct is actually a FDictSpace
    if (!InDictSpace.IsValid())
    {
        RaiseInvalidInstancedStructError(TEXT("DictSpace_Length"));
        return 0;
    }

    const FDictSpace* TypedSpace = InDictSpace.GetPtr<FDictSpace>();
    
    if (!TypedSpace)
    {
        RaiseInstancedStructTypeMismatchError(InDictSpace, TEXT("FDictSpace"), TEXT("DictSpace_Length"));
        return 0;
    }

    return TypedSpace->Spaces.Num();
}

void UDictSpaceBlueprintLibrary::DictSpace_Clear(TInstancedStruct<FDictSpace>& InOutDictSpace)
{
    // Type check: ensure the InstancedStruct is actually a FDictSpace
    if (!InOutDictSpace.IsValid())
    {
        RaiseInvalidInstancedStructError(TEXT("DictSpace_Clear"));
        return;
    }

    FDictSpace* TypedSpace = InOutDictSpace.GetMutablePtr<FDictSpace>();
    
    if (!TypedSpace)
    {
        RaiseInstancedStructTypeMismatchError(InOutDictSpace, TEXT("FDictSpace"), TEXT("DictSpace_Clear"));
        return;
    }

    TypedSpace->Spaces.Empty();
}

void UDictSpaceBlueprintLibrary::DictSpace_Keys(const TInstancedStruct<FDictSpace>& InDictSpace, TArray<FString>& OutKeys)
{
    // Type check: ensure the InstancedStruct is actually a FDictSpace
    if (!InDictSpace.IsValid())
    {
        RaiseInvalidInstancedStructError(TEXT("DictSpace_Keys"));
        OutKeys.Empty();
        return;
    }

    const FDictSpace* TypedSpace = InDictSpace.GetPtr<FDictSpace>();
    
    if (!TypedSpace)
    {
        RaiseInstancedStructTypeMismatchError(InDictSpace, TEXT("FDictSpace"), TEXT("DictSpace_Keys"));
        OutKeys.Empty();
        return;
    }

    TypedSpace->Spaces.GetKeys(OutKeys);
}

void UDictSpaceBlueprintLibrary::DictSpace_Values(const TInstancedStruct<FDictSpace>& InDictSpace, TArray<FInstancedStruct>& OutValues)
{
    // Type check: ensure the InstancedStruct is actually a FDictSpace
    if (!InDictSpace.IsValid())
    {
        RaiseInvalidInstancedStructError(TEXT("DictSpace_Values"));
        OutValues.Empty();
        return;
    }

    const FDictSpace* TypedSpace = InDictSpace.GetPtr<FDictSpace>();
    
    if (!TypedSpace)
    {
        RaiseInstancedStructTypeMismatchError(InDictSpace, TEXT("FDictSpace"), TEXT("DictSpace_Values"));
        OutValues.Empty();
        return;
    }

    OutValues.Empty();
    for (const auto& Pair : TypedSpace->Spaces)
    {
        OutValues.Add(ToUntypedInstancedStruct(Pair.Value));
    }
}
