// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "StructUtils/InstancedStruct.h"
#include "Points/Point.h"
#include "Points/BoxPoint.h"
#include "BoxPointBlueprintLibrary.generated.h"

/**
 * @class UBoxPointBlueprintLibrary
 * @brief Blueprint oriented helper functions for creating & inspecting Box Point InstancedStructs.
 * 
 * This library provides utility functions for creating and manipulating Box (continuous) Point instances
 * from within Blueprints. These return TInstancedStruct<FBoxPoint>.
 */
UCLASS()
class SCHOLA_API UBoxPointBlueprintLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:

    /**
     * @brief Converts an array of float values to a box (continuous) point.
     * @param[in] InValues Array of float values representing continuous values in a box space.
     * @return A new box point instance.
     */
	UFUNCTION(BlueprintPure, Category = "Schola|Point|Box", meta = (DisplayName = "From Array (Box Point)", AutoCreateRefTerm = "InValues"))
	static TInstancedStruct<FBoxPoint> ArrayToBoxPoint(UPARAM(DisplayName="Values") const TArray<float>& InValues);

    /**
     * @brief Converts an array of float values to a box (continuous) point with a specific shape.
     * @param[in] InValues Array of float values representing continuous values.
     * @param[in] InShape The dimensional shape of the box point.
     * @return A new shaped box point instance.
     */
	UFUNCTION(BlueprintPure, Category = "Schola|Point|Box", meta = (DisplayName = "From Array With Shape (Box Point)", AutoCreateRefTerm = "InShape"))
    static TInstancedStruct<FBoxPoint> ArrayToBoxPointShaped(UPARAM(DisplayName="Values") const TArray<float>& InValues, UPARAM(DisplayName="Shape") const TArray<int>& InShape);

    /**
     * @brief Converts a box point to an array of float values.
     * @param[in] InBoxPoint The box point to convert.
     * @return Array of float values from the box point.
     */
    UFUNCTION(BlueprintPure, Category="Schola|Point|Box", meta=(BlueprintAutocast, DisplayName="To Array (Box Point)", CompactNodeTitle="->"))
    static TArray<float> BoxPointToArray(UPARAM(DisplayName="Box Point") const TInstancedStruct<FBoxPoint>& InBoxPoint);

    /**
     * @brief Converts an FVector to a box point.
     * @param[in] InVector The FVector to convert (X, Y, Z components).
     * @return A box point with 3 values representing the vector components.
     */
    UFUNCTION(BlueprintPure, Category = "Schola|Point|Box", meta = (DisplayName = "From Vector (Box Point)"))
    static TInstancedStruct<FBoxPoint> VectorToBoxPoint(UPARAM(DisplayName="Vector") const FVector& InVector);

    /**
     * @brief Converts a box point to an FVector.
     * @param[in] InBoxPoint The box point to convert (must have exactly 3 values).
     * @return An FVector with X, Y, Z components from the box point values.
     */
    UFUNCTION(BlueprintPure, Category = "Schola|Point|Box", meta = (BlueprintAutocast, DisplayName = "To Vector (Box Point)", CompactNodeTitle = "->"))
    static FVector BoxPointToVector(UPARAM(DisplayName="Box Point") const TInstancedStruct<FBoxPoint>& InBoxPoint);

    /**
     * @brief Converts an FRotator to a box point.
     * @param[in] InRotator The FRotator to convert (Pitch, Yaw, Roll components).
     * @return A box point with 3 values representing the rotator components.
     */
    UFUNCTION(BlueprintPure, Category = "Schola|Point|Box", meta = (DisplayName = "From Rotator (Box Point)"))
    static TInstancedStruct<FBoxPoint> RotatorToBoxPoint(UPARAM(DisplayName="Rotator") const FRotator& InRotator);

    /**
     * @brief Converts a box point to an FRotator.
     * @param[in] InBoxPoint The box point to convert (must have exactly 3 values).
     * @return An FRotator with Pitch, Yaw, Roll components from the box point values.
     */
    UFUNCTION(BlueprintPure, Category = "Schola|Point|Box", meta = (BlueprintAutocast, DisplayName = "To Rotator (Box Point)", CompactNodeTitle = "->"))
    static FRotator BoxPointToRotator(UPARAM(DisplayName="Box Point") const TInstancedStruct<FBoxPoint>& InBoxPoint);

    /**
     * @brief Converts an FTransform to a box point.
     * @param[in] InTransform The FTransform to convert (Location, Rotation, Scale).
     * @return A box point with 9 values: [Location.X, Location.Y, Location.Z, Rotation.Pitch, Rotation.Yaw, Rotation.Roll, Scale.X, Scale.Y, Scale.Z].
     */
    UFUNCTION(BlueprintPure, Category = "Schola|Point|Box", meta = (DisplayName = "From Transform (Box Point)"))
    static TInstancedStruct<FBoxPoint> TransformToBoxPoint(UPARAM(DisplayName="Transform") const FTransform& InTransform);

    /**
     * @brief Converts a box point to an FTransform.
     * @param[in] InBoxPoint The box point to convert (must have exactly 9 values).
     * @return An FTransform with Location, Rotation, and Scale components from the box point values.
     */
    UFUNCTION(BlueprintPure, Category = "Schola|Point|Box", meta = (BlueprintAutocast, DisplayName = "To Transform (Box Point)", CompactNodeTitle = "->"))
    static FTransform BoxPointToTransform(UPARAM(DisplayName="Box Point") const TInstancedStruct<FBoxPoint>& InBoxPoint);

};
