// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "ImitationSettings/ImitationSettings.h"
#include "TrainingUtils/ArgBuilder.h"
#include "ImitationLoggingSettings.generated.h"

/**
 * @brief Configuration struct for imitation learning logging settings.
 * @details Generic logging settings that can be used across different imitation learning frameworks.
 * Contains settings for controlling verbosity and output during data collection.
 */
USTRUCT(BlueprintType)
struct SCHOLAIMITATION_API FImitationLoggingSettings : public FImitationSettings
{
	GENERATED_BODY()

public:
	/** The verbosity level for Schola-specific logging (0-2). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0", ClampMax = "2"), Category = "Imitation Logging Settings")
	int32 ScholaVerbosity = 0;

	/**
	 * @brief Generate command-line arguments for logging settings.
	 * @param[in,out] ArgBuilder The argument builder to populate with settings.
	 */
	virtual void GenerateImitationArgs(FScriptArgBuilder& ArgBuilder) const override;

	virtual ~FImitationLoggingSettings();
};
