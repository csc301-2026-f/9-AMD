// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "TrainingSettings/TrainingSettings.h"
#include "TrainingUtils/ArgBuilder.h"
#include "RLlibLoggingSettings.generated.h"

/**
 * @brief A struct to hold logging settings for an RLlib training script
 */
USTRUCT(BlueprintType)
struct SCHOLATRAINING_API FRLlibLoggingSettings : public FTrainingSettings
{
	GENERATED_BODY()

public:
	/** The verbosity of the environment logging, i.e. Schola logs */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0", ClampMax = "2"), Category = "Logging Settings")
	int32 EnvLoggingVerbosity = 0;

	/** The verbosity of the trainer logging. i.e. Ray Logs */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "0", ClampMax = "2"), Category = "Logging Settings")
	int32 TrainerLoggingVerbosity = 1;

	void GenerateTrainingArgs( FScriptArgBuilder& ArgBuilder) const;

	virtual ~FRLlibLoggingSettings();
};