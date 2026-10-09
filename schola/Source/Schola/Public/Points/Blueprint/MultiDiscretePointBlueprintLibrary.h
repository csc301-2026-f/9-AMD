// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "StructUtils/InstancedStruct.h"
#include "Points/Point.h"
#include "Points/MultiDiscretePoint.h"
#include "MultiDiscretePointBlueprintLibrary.generated.h"

/**
 * @class UMultiDiscretePointBlueprintLibrary
 * @brief Blueprint oriented helper functions for creating & inspecting Multi Discrete Point InstancedStructs.
 * 
 * This library provides utility functions for creating and manipulating Multi Discrete Point instances
 * from within Blueprints. These return TInstancedStruct<FMultiDiscretePoint>.
 */
UCLASS()
class SCHOLA_API UMultiDiscretePointBlueprintLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:

    /**
     * @brief Converts an array of integer values to a multi discrete point.
     * @param[in] InValues Array of integer values, each representing a discrete choice.
     * @return A new multi discrete point instance.
     */
    UFUNCTION(BlueprintPure, Category="Schola|Point|MultiDiscrete", meta=(DisplayName="From Array (Multi Discrete Point)", AutoCreateRefTerm="InValues"))
    static TInstancedStruct<FMultiDiscretePoint> ArrayToMultiDiscretePoint(UPARAM(DisplayName="Values") const TArray<int32>& InValues);

    /**
     * @brief Converts a multi discrete point to an array of integer values.
     * @param[in] InMultiDiscretePoint The multi discrete point to convert.
     * @return Array of integer values from the multi discrete point.
     */
    UFUNCTION(BlueprintPure, Category="Schola|Point|MultiDiscrete", meta=(BlueprintAutocast, DisplayName="To Array (Multi Discrete Point)", CompactNodeTitle="->"))
    static TArray<int32> MultiDiscretePointToArray(UPARAM(DisplayName="Multi Discrete Point") const TInstancedStruct<FMultiDiscretePoint>& InMultiDiscretePoint);

};
