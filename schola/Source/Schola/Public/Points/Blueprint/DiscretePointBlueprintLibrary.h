// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "StructUtils/InstancedStruct.h"
#include "Points/Point.h"
#include "Points/DiscretePoint.h"
#include "DiscretePointBlueprintLibrary.generated.h"

/**
 * @class UDiscretePointBlueprintLibrary
 * @brief Blueprint oriented helper functions for creating & inspecting Discrete Point InstancedStructs.
 * 
 * This library provides utility functions for creating and manipulating Discrete Point instances
 * from within Blueprints. These return TInstancedStruct<FDiscretePoint>.
 */
UCLASS()
class SCHOLA_API UDiscretePointBlueprintLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:

    /**
     * @brief Converts an integer value to a discrete point.
     * @param[in] InValue The integer value representing a discrete choice.
     * @return A new discrete point instance.
     */
	UFUNCTION(BlueprintPure, Category = "Schola|Point|Discrete", meta = (DisplayName = "From Int32 (Discrete Point)"))
	static TInstancedStruct<FDiscretePoint> Int32ToDiscretePoint(UPARAM(DisplayName="Value") const int32 InValue);

    /**
     * @brief Converts a discrete point to an integer value.
     * @param[in] InDiscretePoint The discrete point to convert.
     * @return The integer value from the discrete point.
     */
    UFUNCTION(BlueprintPure, Category="Schola|Point|Discrete", meta=(BlueprintAutocast, DisplayName="To Int32 (Discrete Point)", CompactNodeTitle="->"))
    static int32 DiscretePointToInt32(UPARAM(DisplayName="Discrete Point") const TInstancedStruct<FDiscretePoint>& InDiscretePoint);

};
