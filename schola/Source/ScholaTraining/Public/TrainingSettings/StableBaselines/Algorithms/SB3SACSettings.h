// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "TrainingSettings/TrainingSettings.h"
#include "TrainingUtils/ArgBuilder.h"
#include "SB3SACSettings.generated.h"


/**
 * @brief A struct to hold SAC settings for an SB3 training script
 * @note This is a partial implementation of the SAC settings, and is not exhaustive
 */
USTRUCT(BlueprintType)
struct SCHOLATRAINING_API FSB3SACSettings : public FTrainingSettings
{
	GENERATED_BODY()

public:
	/** The learning rate for the SAC algorithm */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 0.0), Category = "SAC Settings")
	float LearningRate = 0.0003;

	/** The buffer size for the SAC algorithm */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SAC Settings")
	int BufferSize = 1000000;

	/** The number of steps to take before learning starts */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SAC Settings")
	int LearningStarts = 100;

	/** The batch size to use during gradient descent */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SAC Settings")
	int BatchSize = 256;

	/** The Tau value for the SAC algorithm */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SAC Settings")
	float Tau = 0.005;

	/** The gamma value for the SAC algorithm */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SAC Settings")
	float Gamma = 0.99;

	/** The frequency to update the target network, in steps */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SAC Settings")
	int TrainFreq = 1;

	/** The number of gradient steps to take during training */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SAC Settings")
	int GradientSteps = 1;

	/** Optimize memory usage */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SAC Settings")
	bool OptimizeMemoryUsage = false;

	/** Should we learn the entropy coefficient during training */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SAC Settings")
	bool LearnEntCoef = true;

	/** The initial entropy coefficient for the SAC algorithm */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SAC Settings")
	float InitialEntCoef = 1.0;

	/** The interval at which we update the target network */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SAC Settings")
	int TargetUpdateInterval = 1;

	/** The target entropy for the SAC algorithm. use auto to learn the target entropy */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SAC Settings")
	FString TargetEntropy = "auto";

	/** Use state dependent entropy noise */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SAC Settings")
	bool UseSDE = false;

	/** The frequency to sample the state dependent entropy noise */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SAC Settings")
	int SDESampleFreq = -1;

	void GenerateTrainingArgs(FScriptArgBuilder& ArgBuilder) const;

	virtual ~FSB3SACSettings();
};