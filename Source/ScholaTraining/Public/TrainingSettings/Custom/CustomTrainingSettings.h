// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "TrainingUtils/ArgBuilder.h"
#include "TrainingSettings/TrainingSettings.h"
#include "CustomTrainingSettings.generated.h"

/**
 * @brief Configuration struct for custom user-provided training scripts.
 * @details Allows users to specify their own training scripts with custom arguments and flags.
 * Useful for integrating custom RL frameworks or training methods not covered by built-in options.
 */
USTRUCT(BlueprintType)
struct SCHOLATRAINING_API FCustomTrainingSettings : public FTrainingSettings
{
	GENERATED_BODY()

public:
	/** The file system path to the custom script to launch. */
	UPROPERTY(Config, EditAnywhere, Category = "Script Settings|Custom Script")
	FFilePath LaunchScript;

	/** Map of argument names to values to pass to the custom script. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Script Settings|Custom Script")
	TMap<FString, FString> Args;

	/** Array of command-line flags (boolean options) to pass to the custom script. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Script Settings|Custom Script")
	TArray<FString> Flags;

	/**
	 * @brief Generate command-line arguments for the custom training script.
	 * @param[in,out] ArgBuilder The argument builder to populate with settings.
	 */
	void GenerateTrainingArgs( FScriptArgBuilder& ArgBuilder) const;

	/**
	 * @brief Virtual destructor.
	 */
	virtual ~FCustomTrainingSettings();
};