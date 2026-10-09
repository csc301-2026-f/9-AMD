// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "Points/Blueprint/DictPointBlueprintLibrary.h"
#include "Points/DictPoint.h"
#include "Common/BlueprintErrorUtils.h"
#include "Common/InstancedStructUtils.h"

TInstancedStruct<FDictPoint> UDictPointBlueprintLibrary::MapToDictPoint(const TMap<FString, FInstancedStruct>& InPoints)
{
	// Convert FInstancedStruct to TInstancedStruct<FPoint> for FDictPoint constructor
	return TInstancedStruct<FDictPoint>::Make<FDictPoint>(reinterpret_cast<const TMap<FString,TInstancedStruct<FPoint>>&>(InPoints));
}

TMap<FString, FInstancedStruct> UDictPointBlueprintLibrary::DictPointToMap(const TInstancedStruct<FDictPoint>& InDictPoint)
{
    TMap<FString, FInstancedStruct> Result;
    
    // Type check: ensure the InstancedStruct is actually a FDictPoint
    if (!InDictPoint.IsValid())
    {
        RaiseInvalidInstancedStructError(TEXT("DictPointToMap"));
        return Result;
    }

    const FDictPoint* TypedPoint = InDictPoint.GetPtr<FDictPoint>();

    if (!TypedPoint)
	{
        RaiseInstancedStructTypeMismatchError(InDictPoint, TEXT("FDictPoint"), TEXT("DictPointToMap"));
        return Result;
    }

    return reinterpret_cast<const TMap<FString,FInstancedStruct>&>(TypedPoint->Points);
}

bool UDictPointBlueprintLibrary::DictPoint_Add(TInstancedStruct<FDictPoint>& InOutDictPoint, const FString& InKey, const FInstancedStruct& InValue)
{
    // Type check: ensure the InstancedStruct is actually a FDictPoint
    if (!InOutDictPoint.IsValid())
    {
        RaiseInvalidInstancedStructError(TEXT("DictPoint_Add"));
        return false;
    }

    FDictPoint* TypedPoint = InOutDictPoint.GetMutablePtr<FDictPoint>();
    
    if (!TypedPoint)
    {
        RaiseInstancedStructTypeMismatchError(InOutDictPoint, TEXT("FDictPoint"), TEXT("DictPoint_Add"));
        return false;
    }

    if (!InValue.GetScriptStruct())
    {
        RaiseInvalidInstancedStructError(TEXT("DictPoint_Add"));
        return false;
    }

    if (!InValue.GetScriptStruct()->IsChildOf(FPoint::StaticStruct()))
    {
        RaiseInstancedStructTypeMismatchError(InValue, TEXT("FPoint"), TEXT("DictPoint_Add"));
        return false;
    }
	
	TypedPoint->Points.Add(InKey, ToTypedInstancedStruct<FPoint>(InValue));

    return true;
}

bool UDictPointBlueprintLibrary::DictPoint_Find(TInstancedStruct<FDictPoint>& InDictPoint, const FString& InKey, FInstancedStruct& OutValue)
{
    // Type check: ensure the InstancedStruct is actually a FDictPoint
    if (!InDictPoint.IsValid())
    {
        RaiseInvalidInstancedStructError(TEXT("DictPoint_Find"));
        return false;
    }

    FDictPoint* TypedPoint = InDictPoint.GetMutablePtr<FDictPoint>();
    
    if (!TypedPoint)
    {
        RaiseInstancedStructTypeMismatchError(InDictPoint, TEXT("FDictPoint"), TEXT("DictPoint_Find"));
        return false;
    }

    TInstancedStruct<FPoint>* Found = TypedPoint->Points.Find(InKey);

    if (!Found)
    {
        return false;
    }
    else
    {
		OutValue = ToUntypedInstancedStruct(*Found);
		return true;
    }

}

bool UDictPointBlueprintLibrary::DictPoint_Contains(const TInstancedStruct<FDictPoint>& InDictPoint, const FString& InKey)
{
    // Type check: ensure the InstancedStruct is actually a FDictPoint
    if (!InDictPoint.IsValid())
    {
        RaiseInvalidInstancedStructError(TEXT("DictPoint_Contains"));
        return false;
    }

    const FDictPoint* TypedPoint = InDictPoint.GetPtr<FDictPoint>();
    
    if (!TypedPoint)
    {
        RaiseInstancedStructTypeMismatchError(InDictPoint, TEXT("FDictPoint"), TEXT("DictPoint_Contains"));
        return false;
    }
    
    return TypedPoint->Points.Contains(InKey);
}

bool UDictPointBlueprintLibrary::DictPoint_Remove(TInstancedStruct<FDictPoint>& InOutDictPoint, const FString& InKey)
{
    // Type check: ensure the InstancedStruct is actually a FDictPoint
    if (!InOutDictPoint.IsValid())
    {
        RaiseInvalidInstancedStructError(TEXT("DictPoint_Remove"));
        return false;
    }

    FDictPoint* TypedPoint = InOutDictPoint.GetMutablePtr<FDictPoint>();
    
    if (!TypedPoint) 
    {
        RaiseInstancedStructTypeMismatchError(InOutDictPoint, TEXT("FDictPoint"), TEXT("DictPoint_Remove"));
        return false;
    }

    return TypedPoint->Points.Remove(InKey) > 0;
}

int32 UDictPointBlueprintLibrary::DictPoint_Length(const TInstancedStruct<FDictPoint>& InDictPoint)
{
    // Type check: ensure the InstancedStruct is actually a FDictPoint
    if (!InDictPoint.IsValid())
    {
        RaiseInvalidInstancedStructError(TEXT("DictPoint_Length"));
        return 0;
    }

    const FDictPoint* TypedPoint = InDictPoint.GetPtr<FDictPoint>();
    
    if (!TypedPoint)
    {
        RaiseInstancedStructTypeMismatchError(InDictPoint, TEXT("FDictPoint"), TEXT("DictPoint_Length"));
        return 0;
    }

    return TypedPoint->Points.Num();
}

void UDictPointBlueprintLibrary::DictPoint_Clear(TInstancedStruct<FDictPoint>& InOutDictPoint)
{
    // Type check: ensure the InstancedStruct is actually a FDictPoint
    if (!InOutDictPoint.IsValid())
    {
        RaiseInvalidInstancedStructError(TEXT("DictPoint_Clear"));
        return;
    }

    FDictPoint* TypedPoint = InOutDictPoint.GetMutablePtr<FDictPoint>();
    
    if (!TypedPoint) 
    {
        RaiseInstancedStructTypeMismatchError(InOutDictPoint, TEXT("FDictPoint"), TEXT("DictPoint_Clear"));
        return;
    }

    TypedPoint->Points.Empty();
}

void UDictPointBlueprintLibrary::DictPoint_Keys(const TInstancedStruct<FDictPoint>& InDictPoint, TArray<FString>& OutKeys)
{
    // Type check: ensure the InstancedStruct is actually a FDictPoint
    if (!InDictPoint.IsValid())
    {
        RaiseInvalidInstancedStructError(TEXT("DictPoint_Keys"));
        OutKeys.Empty();
        return;
    }

    const FDictPoint* TypedPoint = InDictPoint.GetPtr<FDictPoint>();
    
    if (!TypedPoint) 
    {
        RaiseInstancedStructTypeMismatchError(InDictPoint, TEXT("FDictPoint"), TEXT("DictPoint_Keys"));
        OutKeys.Empty();
        return;
    }

    TypedPoint->Points.GetKeys(OutKeys);
}

void UDictPointBlueprintLibrary::DictPoint_Values(const TInstancedStruct<FDictPoint>& InDictPoint, TArray<FInstancedStruct>& OutValues)
{
    // Type check: ensure the InstancedStruct is actually a FDictPoint
    if (!InDictPoint.IsValid())
    {
        RaiseInvalidInstancedStructError(TEXT("DictPoint_Values"));
        OutValues.Empty();
        return;
    }

    const FDictPoint* TypedPoint = InDictPoint.GetPtr<FDictPoint>();
    
    if (!TypedPoint) 
    {
        RaiseInstancedStructTypeMismatchError(InDictPoint, TEXT("FDictPoint"), TEXT("DictPoint_Values"));
        OutValues.Empty();
        return;
    }

    OutValues.Empty();
    for (const auto& Pair : TypedPoint->Points)
    {
        OutValues.Add(ToUntypedInstancedStruct(Pair.Value));
    }
}
