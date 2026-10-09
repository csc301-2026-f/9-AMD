// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "Common/LogSchola.h"

/**
 * @brief A class to build arguments and flags for a CLI script.
 */
struct SCHOLATRAINING_API FScriptArgBuilder
{
	
public:
	/** The arguments to be passed to the commnand line script */
	TArray<FString> Args;

	/**
	 * @brief Add a string argument to the command line
	 * @param[in] ArgName The name of the argument
	 * @param[in] ArgValue The value of the argument
	 */
	FScriptArgBuilder& AddStringArg(FString ArgName, FString ArgValue);

	/**
	 * @brief Add a float argument to the command line
	 * @param[in] ArgName The name of the argument
	 * @param[in] ArgValue The value of the argument. Will be converted to a string
	 */
	FScriptArgBuilder& AddFloatArg(FString ArgName, float ArgValue);

	/**
	 * @brief Add an integer argument to the command line
	 * @param[in] ArgName The name of the argument
	 * @param[in] ArgValue The value of the argument. Will be converted to a string
	 */
	FScriptArgBuilder& AddIntArg(FString ArgName, int ArgValue);

	/**
	 * @brief Add a flag to the command line
	 * @param[in] FlagName The name of the flag
	 * @param[in] bCondition Whether the flag should be added. Useful programmatically setting conditional flags.
	 */
	FScriptArgBuilder& AddFlag(FString FlagName, bool bCondition = true);

	/**
	 * @brief Add a string argument to the command line if a condition is met
	 * @param[in] ArgName The name of the argument
	 * @param[in] ArgValue The value of the argument
	 * @param[in] bCondition Whether the argument should be added. Useful programmatically setting conditional arguments.
	 */
	FScriptArgBuilder& AddConditionalStringArg(FString ArgName, FString ArgValue, bool bCondition);
	
	/**
	 * @brief Add an integer array argument to the command line
	 * @param[in] ArgName The name of the argument
	 * @param[in] ArgValue The value of the argument
	 * @note The array will be converted to a string by converting to a string array and putting spaces between each value
	 */
	FScriptArgBuilder& AddIntArrayArg(FString ArgName, const TArray<int>& ArgValue);
	
	/**
	 * @brief Add a positional argument (i.e. does not append - infront of the name)
	 * @param[in] Arg The argument to add
	 */
	FScriptArgBuilder& AddPositionalArgument(FString Arg);

	/**
	 * @brief Construct the command line string from the added arguments
	 * @return The command line string
	 */
	FString Build();
};