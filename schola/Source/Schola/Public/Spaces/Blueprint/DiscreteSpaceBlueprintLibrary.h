// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "StructUtils/InstancedStruct.h"
#include "Spaces/Space.h"
#include "Spaces/DiscreteSpace.h"
#include "DiscreteSpaceBlueprintLibrary.generated.h"

/**
 * @class UDiscreteSpaceBlueprintLibrary
 * @brief Blueprint oriented helper functions for creating & inspecting Discrete Space InstancedStructs.
 * 
 * This library provides utility functions for creating and manipulating Discrete Space instances
 * from within Blueprints. These return TInstancedStruct<FDiscreteSpace>.
 */
UCLASS()
class SCHOLA_API UDiscreteSpaceBlueprintLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:

    /**
     * @brief Converts an integer to a discrete space with a specified upper bound.
     * @param[in] InHigh The maximum value (exclusive) in the action space.
     * @return A new discrete space instance.
     */
    UFUNCTION(BlueprintPure, Category = "Schola|Space|Discrete", meta = (DisplayName = "From Int32 (Discrete Space)"))
	static TInstancedStruct<FDiscreteSpace> Int32ToDiscreteSpace(UPARAM(DisplayName="High") int32 InHigh);

    /**
     * @brief Converts a discrete space to its upper bound.
     * @param[in] InDiscreteSpace The discrete space to convert.
     * @return The upper bound (exclusive) of the discrete space.
     */
    UFUNCTION(BlueprintPure, Category="Schola|Space|Discrete", meta=(BlueprintAutocast, DisplayName="To Int32 (Discrete Space)", CompactNodeTitle="->"))
    static int32 DiscreteSpaceToInt32(UPARAM(DisplayName="Discrete Space") const TInstancedStruct<FDiscreteSpace>& InDiscreteSpace);

};
