// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "StructUtils/InstancedStruct.h"
#include "Spaces/MultiDiscreteSpace.h"
#include "MultiDiscreteSpaceBlueprintLibrary.generated.h"

/**
 * @class UMultiDiscreteSpaceBlueprintLibrary
 * @brief Blueprint oriented helper functions for creating & inspecting Multi Discrete Space InstancedStructs.
 * 
 * This library provides utility functions for creating and manipulating Multi Discrete Space instances
 * from within Blueprints. These return TInstancedStruct<FMultiDiscreteSpace>.
 */
UCLASS()
class SCHOLA_API UMultiDiscreteSpaceBlueprintLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:

    /**
     * @brief Converts an array to a multi-discrete space with specified upper bounds.
     * @param[in] InHigh Array of upper bounds (exclusive) for each discrete dimension.
     * @return A new multi-discrete space instance.
     */
    UFUNCTION(BlueprintPure, Category="Schola|Space|MultiDiscrete", meta=(DisplayName="From Array (Multi Discrete Space)"))
    static TInstancedStruct<FMultiDiscreteSpace> ArrayToMultiDiscreteSpace(UPARAM(DisplayName="High") const TArray<int32>& InHigh);

    /**
     * @brief Converts a multi-discrete space to an array of upper bounds.
     * @param[in] InMultiDiscreteSpace The multi-discrete space to convert.
     * @return Array of upper bounds from the multi-discrete space.
     */
    UFUNCTION(BlueprintPure, Category="Schola|Space|MultiDiscrete", meta=(BlueprintAutocast, DisplayName="To Array (Multi Discrete Space)", CompactNodeTitle="->"))
    static TArray<int32> MultiDiscreteSpaceToArray(UPARAM(DisplayName="Multi Discrete Space") const TInstancedStruct<FMultiDiscreteSpace>& InMultiDiscreteSpace);

};
