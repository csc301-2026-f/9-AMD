// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TrainingUtils/LaunchableScript.h"
#include "LaunchableScriptFunctionLibrary.generated.h"

/**
 * @brief Blueprint function library for launching external scripts.
 * @details Provides Blueprint-accessible functions for managing script execution.
 */
UCLASS()
class SCHOLATRAINING_API ULaunchableScriptFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    /**
     * @brief Launches the script described by the LaunchableScript struct.
     * @param[in,out] InScript The script configuration to launch.
     * @details This function starts the script in a separate process and returns immediately.
     * The script will continue running in the background.
     */
    UFUNCTION(BlueprintCallable, Category = "Script")
	static void LaunchScript(UPARAM(ref) FLaunchableScript& InScript)
    {
		InScript.LaunchScript();
    }

    /**
     * @brief Kills the currently running script process.
     * @param[in,out] InScript The script instance whose process should be terminated.
     * @details This function terminates the running script process and cleans up resources.
     * @return True if the process was successfully terminated or wasn't running, false on error.
     */
    UFUNCTION(BlueprintCallable, Category = "Script")
	static bool KillScript(UPARAM(ref) FLaunchableScript& InScript)
    {
		return InScript.KillScript();
    }

};
