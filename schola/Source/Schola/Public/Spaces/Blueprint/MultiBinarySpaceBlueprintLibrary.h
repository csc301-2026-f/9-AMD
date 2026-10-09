// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "StructUtils/InstancedStruct.h"
#include "Spaces/Space.h"
#include "Spaces/MultiBinarySpace.h"
#include "MultiBinarySpaceBlueprintLibrary.generated.h"

/**
 * @class UMultiBinarySpaceBlueprintLibrary
 * @brief Blueprint oriented helper functions for creating & inspecting MultiBinary Space InstancedStructs.
 * 
 * This library provides utility functions for creating and manipulating MultiBinary Space instances
 * from within Blueprints. These return TInstancedStruct<FMultiBinarySpace>.
 */
UCLASS()
class SCHOLA_API UMultiBinarySpaceBlueprintLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:

    /**
     * @brief Creates a MultiBinary space with a specified number of bits.
     * @param[in] InShape The number of binary values (bits) in the space.
     * @return A new MultiBinary space instance.
     */
    UFUNCTION(BlueprintPure, Category="Schola|Space|MultiBinary", meta=(DisplayName="From Shape (Multi Binary Space)"))
    static TInstancedStruct<FMultiBinarySpace> ShapeToMultiBinarySpace(UPARAM(DisplayName="Shape") int32 InShape);

    /**
     * @brief Gets the shape (number of bits) from a MultiBinary space.
     * @param[in] InMultiBinarySpace The MultiBinary space to query.
     * @return The shape (number of bits) of the MultiBinary space.
     */
    UFUNCTION(BlueprintPure, Category="Schola|Space|MultiBinary", meta=(DisplayName="Get Shape (Multi Binary Space)", ReturnDisplayName="Shape"))
    static int32 MultiBinarySpace_GetShape(UPARAM(DisplayName="Multi Binary Space") const TInstancedStruct<FMultiBinarySpace>& InMultiBinarySpace);

};
