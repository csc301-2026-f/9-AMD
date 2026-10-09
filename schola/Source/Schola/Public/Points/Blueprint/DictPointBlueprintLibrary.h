// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "StructUtils/InstancedStruct.h"
#include "Points/Point.h"
#include "Points/DictPoint.h"
#include "DictPointBlueprintLibrary.generated.h"

/**
 * @class UDictPointBlueprintLibrary
 * @brief Blueprint oriented helper functions for creating & inspecting Dict Point InstancedStructs.
 * 
 * This library provides utility functions for creating and manipulating Dictionary Point instances
 * from within Blueprints. These return TInstancedStruct<FDictPoint>.
 */
UCLASS()
class SCHOLA_API UDictPointBlueprintLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:

    /**
     * @brief Converts a map to a dictionary point.
     * @param[in] InPoints Map of key-value pairs where keys are strings and values are sub-points.
     * @return A new dictionary point instance that can hold named sub-points.
     */
	UFUNCTION(BlueprintPure, Category = "Schola|Point|Dict", meta = (AutoCreateRefTerm = "InPoints", DisplayName = "From Map (Dict Point)"))
    static TInstancedStruct<FDictPoint> MapToDictPoint(UPARAM(DisplayName="Points") const TMap<FString, FInstancedStruct>& InPoints);

    /**
     * @brief Converts a dictionary point to a map of sub-points.
     * @param[in] InDictPoint The dictionary point to convert.
     * @return Map of named sub-points from the dictionary point.
     */
    UFUNCTION(BlueprintPure, Category="Schola|Point|Dict", meta=(BlueprintAutocast, DisplayName="To Map (Dict Point)", CompactNodeTitle="->"))
    static TMap<FString, FInstancedStruct> DictPointToMap(UPARAM(DisplayName="Dict Point") const TInstancedStruct<FDictPoint>& InDictPoint);

    /**
     * @brief Adds or updates a sub-point inside a dictionary point.
     * @param[in,out] InOutDictPoint The dictionary point to modify (passed by reference).
     * @param[in] InKey The key name for the sub-point.
     * @param[in] InValue The sub-point to store.
     * @return True if the operation succeeded, false otherwise.
     */
    UFUNCTION(BlueprintCallable, Category="Schola|Point|Dict", meta=(AutoCreateRefTerm="InValue", DisplayName="Add (Dict Point)", ReturnDisplayName="Added"))
    static bool DictPoint_Add(UPARAM(ref, DisplayName="Dict Point") TInstancedStruct<FDictPoint>& InOutDictPoint, UPARAM(DisplayName="Key") const FString& InKey, UPARAM(DisplayName="Value") const FInstancedStruct& InValue);

    /**
     * @brief Finds a sub-point in a dictionary point by key.
     * @param[in] InDictPoint The dictionary point to query.
     * @param[in] InKey The key name of the sub-point to retrieve.
     * @param[out] OutValue Output parameter that receives the sub-point if found.
     * @return True if the sub-point was found, false otherwise.
     */
    UFUNCTION(BlueprintPure, Category="Schola|Point|Dict", meta=(DisplayName="Find (Dict Point)", ReturnDisplayName="Found"))
    static bool DictPoint_Find(UPARAM(ref, DisplayName="Dict Point") TInstancedStruct<FDictPoint>& InDictPoint, UPARAM(DisplayName="Key") const FString& InKey, UPARAM(DisplayName="Value") FInstancedStruct& OutValue);

    /**
     * @brief Checks if a dictionary point contains a specific key.
     * @param[in] InDictPoint The dictionary point to query.
     * @param[in] InKey The key name to check for.
     * @return True if the key exists in the dictionary, false otherwise.
     */
    UFUNCTION(BlueprintPure, Category="Schola|Point|Dict", meta=(DisplayName="Contains (Dict Point)", ReturnDisplayName="Contained"))
    static bool DictPoint_Contains(UPARAM(DisplayName="Dict Point") const TInstancedStruct<FDictPoint>& InDictPoint, UPARAM(DisplayName="Key") const FString& InKey);

    /**
     * @brief Removes a sub-point from a dictionary point by key.
     * @param[in,out] InOutDictPoint The dictionary point to modify (passed by reference).
     * @param[in] InKey The key name of the sub-point to remove.
     * @return The number of elements removed (0 or 1).
     */
    UFUNCTION(BlueprintCallable, Category="Schola|Point|Dict", meta=(DisplayName="Remove (Dict Point)", ReturnDisplayName="Removed"))
    static bool DictPoint_Remove(UPARAM(ref, DisplayName="Dict Point") TInstancedStruct<FDictPoint>& InOutDictPoint, UPARAM(DisplayName="Key") const FString& InKey);

    /**
     * @brief Gets the number of entries in a dictionary point.
     * @param[in] InDictPoint The dictionary point to query.
     * @return The number of entries in the dictionary.
     */
    UFUNCTION(BlueprintPure, Category="Schola|Point|Dict", meta=(DisplayName="Length (Dict Point)", ReturnDisplayName="Length"))
    static int32 DictPoint_Length(UPARAM(DisplayName="Dict Point") const TInstancedStruct<FDictPoint>& InDictPoint);

    /**
     * @brief Removes all entries from a dictionary point.
     * @param[in,out] InOutDictPoint The dictionary point to clear (passed by reference).
     */
    UFUNCTION(BlueprintCallable, Category="Schola|Point|Dict", meta=(DisplayName="Clear (Dict Point)"))
    static void DictPoint_Clear(UPARAM(ref, DisplayName="Dict Point") TInstancedStruct<FDictPoint>& InOutDictPoint);

    /**
     * @brief Gets all keys from a dictionary point.
     * @param[in] InDictPoint The dictionary point to query.
     * @param[out] OutKeys Output parameter that receives the array of keys.
     */
    UFUNCTION(BlueprintCallable, Category="Schola|Point|Dict", meta=(DisplayName="Keys (Dict Point)"))
    static void DictPoint_Keys(UPARAM(DisplayName="Dict Point") const TInstancedStruct<FDictPoint>& InDictPoint, UPARAM(DisplayName="Keys") TArray<FString>& OutKeys);

    /**
     * @brief Gets all values from a dictionary point.
     * @param[in] InDictPoint The dictionary point to query.
     * @param[out] OutValues Output parameter that receives the array of sub-points.
     */
    UFUNCTION(BlueprintCallable, Category="Schola|Point|Dict", meta=(DisplayName="Values (Dict Point)"))
    static void DictPoint_Values(UPARAM(DisplayName="Dict Point") const TInstancedStruct<FDictPoint>& InDictPoint, UPARAM(DisplayName="Values") TArray<FInstancedStruct>& OutValues);

};
