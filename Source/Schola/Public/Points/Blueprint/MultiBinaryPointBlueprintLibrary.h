// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "StructUtils/InstancedStruct.h"
#include "Points/Point.h"
#include "Points/MultiBinaryPoint.h"
#include "MultiBinaryPointBlueprintLibrary.generated.h"

/**
 * @class UMultiBinaryPointBlueprintLibrary
 * @brief Blueprint oriented helper functions for creating & inspecting MultiBinary Point InstancedStructs.
 * 
 * This library provides utility functions for creating and manipulating MultiBinary Point instances
 * from within Blueprints. These return TInstancedStruct<FMultiBinaryPoint>.
 */
UCLASS()
class SCHOLA_API UMultiBinaryPointBlueprintLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:

    /**
     * @brief Converts an array of boolean values to a MultiBinary point.
     * @param[in] InValues Array of boolean values representing the MultiBinary point.
     * @return A new MultiBinary point instance.
     */
    UFUNCTION(BlueprintPure, Category="Schola|Point|MultiBinary", meta=(DisplayName="From Array (Multi Binary Point)", AutoCreateRefTerm="InValues"))
    static TInstancedStruct<FMultiBinaryPoint> ArrayToMultiBinaryPoint(UPARAM(DisplayName="Values") const TArray<bool>& InValues);

    /**
     * @brief Converts a MultiBinary point to an array of boolean values.
     * @param[in] InMultiBinaryPoint The MultiBinary point to convert.
     * @return Array of boolean values from the MultiBinary point.
     */
    UFUNCTION(BlueprintPure, Category="Schola|Point|MultiBinary", meta=(BlueprintAutocast, DisplayName="To Array (Multi Binary Point)", CompactNodeTitle="->"))
    static TArray<bool> MultiBinaryPointToArray(UPARAM(DisplayName="Multi Binary Point") const TInstancedStruct<FMultiBinaryPoint>& InMultiBinaryPoint);

};
