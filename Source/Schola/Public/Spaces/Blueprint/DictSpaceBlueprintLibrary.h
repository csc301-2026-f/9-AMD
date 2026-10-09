// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "StructUtils/InstancedStruct.h"
#include "Spaces/Space.h"
#include "Spaces/DictSpace.h"
#include "DictSpaceBlueprintLibrary.generated.h"

/**
 * @class UDictSpaceBlueprintLibrary
 * @brief Blueprint oriented helper functions for creating & inspecting Dict Space InstancedStructs.
 * 
 * This library provides utility functions for creating and manipulating Dictionary Space instances
 * from within Blueprints. These return TInstancedStruct<FDictSpace>.
 */
UCLASS()
class SCHOLA_API UDictSpaceBlueprintLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:

    /**
     * @brief Converts a map to a dictionary space.
     * @param[in] InSpaces Map of key-value pairs where keys are strings and values are sub-spaces.
     * @return A new dictionary space that can hold named sub-spaces.
     */
	UFUNCTION(BlueprintPure, Category = "Schola|Space|Dict", meta = (AutoCreateRefTerm = "InSpaces", DisplayName = "From Map (Dict Space)"))
    static TInstancedStruct<FDictSpace> MapToDictSpace(UPARAM(DisplayName="Spaces") const TMap<FString, FInstancedStruct>& InSpaces);

    /**
     * @brief Converts a dictionary space to a map of sub-spaces.
     * @param[in] InDictSpace The dictionary space to convert.
     * @return Map of named sub-spaces from the dictionary space.
     */
    UFUNCTION(BlueprintPure, Category="Schola|Space|Dict", meta=(BlueprintAutocast, DisplayName="To Map (Dict Space)", CompactNodeTitle="->"))
    static TMap<FString, FInstancedStruct> DictSpaceToMap(UPARAM(DisplayName="Dict Space") const TInstancedStruct<FDictSpace>& InDictSpace);

    /**
     * @brief Adds or updates a sub-space inside a dictionary space.
     * @param[in,out] InOutDictSpace The dictionary space to modify (passed by reference).
     * @param[in] InKey The key name for the sub-space.
     * @param[in] InValue The sub-space to store.
     * @return True if the operation succeeded, false otherwise.
     */
    UFUNCTION(BlueprintCallable, Category="Schola|Space|Dict", meta=(AutoCreateRefTerm="InValue", DisplayName="Add (Dict Space)", ReturnDisplayName="Added"))
    static bool DictSpace_Add(UPARAM(ref, DisplayName="Dict Space") TInstancedStruct<FDictSpace>& InOutDictSpace, UPARAM(DisplayName="Key") const FString& InKey, UPARAM(DisplayName="Value") const FInstancedStruct& InValue);

    /**
     * @brief Finds a sub-space in a dictionary space by key.
     * @param[in] InDictSpace The dictionary space to query.
     * @param[in] InKey The key name of the sub-space to retrieve.
     * @param[out] OutValue Output parameter that receives the sub-space if found.
     * @return True if the sub-space was found, false otherwise.
     */
    UFUNCTION(BlueprintPure, Category="Schola|Space|Dict", meta=(DisplayName="Find (Dict Space)", ReturnDisplayName="Found"))
    static bool DictSpace_Find(UPARAM(DisplayName="Dict Space") const TInstancedStruct<FDictSpace>& InDictSpace, UPARAM(DisplayName="Key") const FString& InKey, UPARAM(DisplayName="Value") FInstancedStruct& OutValue);

    /**
     * @brief Checks if a dictionary space contains a specific key.
     * @param[in] InDictSpace The dictionary space to query.
     * @param[in] InKey The key name to check for.
     * @return True if the key exists in the dictionary, false otherwise.
     */
    UFUNCTION(BlueprintPure, Category="Schola|Space|Dict", meta=(DisplayName="Contains (Dict Space)", ReturnDisplayName="Contained"))
    static bool DictSpace_Contains(UPARAM(DisplayName="Dict Space") const TInstancedStruct<FDictSpace>& InDictSpace, UPARAM(DisplayName="Key") const FString& InKey);

    /**
     * @brief Removes a sub-space from a dictionary space by key.
     * @param[in,out] InOutDictSpace The dictionary space to modify (passed by reference).
     * @param[in] InKey The key name of the sub-space to remove.
     * @return The number of elements removed (0 or 1).
     */
    UFUNCTION(BlueprintCallable, Category="Schola|Space|Dict", meta=(DisplayName="Remove (Dict Space)", ReturnDisplayName="Removed"))
    static bool DictSpace_Remove(UPARAM(ref, DisplayName="Dict Space") TInstancedStruct<FDictSpace>& InOutDictSpace, UPARAM(DisplayName="Key") const FString& InKey);

    /**
     * @brief Gets the number of entries in a dictionary space.
     * @param[in] InDictSpace The dictionary space to query.
     * @return The number of entries in the dictionary.
     */
    UFUNCTION(BlueprintPure, Category="Schola|Space|Dict", meta=(DisplayName="Length (Dict Space)", ReturnDisplayName="Length"))
    static int32 DictSpace_Length(UPARAM(DisplayName="Dict Space") const TInstancedStruct<FDictSpace>& InDictSpace);

    /**
     * @brief Removes all entries from a dictionary space.
     * @param[in,out] InOutDictSpace The dictionary space to clear (passed by reference).
     */
    UFUNCTION(BlueprintCallable, Category="Schola|Space|Dict", meta=(DisplayName="Clear (Dict Space)"))
    static void DictSpace_Clear(UPARAM(ref, DisplayName="Dict Space") TInstancedStruct<FDictSpace>& InOutDictSpace);

    /**
     * @brief Gets all keys from a dictionary space.
     * @param[in] InDictSpace The dictionary space to query.
     * @param[out] OutKeys Output parameter that receives the array of keys.
     */
    UFUNCTION(BlueprintCallable, Category="Schola|Space|Dict", meta=(DisplayName="Keys (Dict Space)"))
    static void DictSpace_Keys(UPARAM(DisplayName="Dict Space") const TInstancedStruct<FDictSpace>& InDictSpace, UPARAM(DisplayName="Keys") TArray<FString>& OutKeys);

    /**
     * @brief Gets all values from a dictionary space.
     * @param[in] InDictSpace The dictionary space to query.
     * @param[out] OutValues Output parameter that receives the array of sub-spaces.
     */
    UFUNCTION(BlueprintCallable, Category="Schola|Space|Dict", meta=(DisplayName="Values (Dict Space)"))
    static void DictSpace_Values(UPARAM(DisplayName="Dict Space") const TInstancedStruct<FDictSpace>& InDictSpace, UPARAM(DisplayName="Values") TArray<FInstancedStruct>& OutValues);

};
