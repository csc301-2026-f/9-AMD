// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "TrainingSettings/TrainingSettings.h"
#include "TrainingUtils/ArgBuilder.h"

#include "SB3PPOSettings.generated.h"

/**
 * @brief A struct to hold PPO settings for an SB3 training script
 * @note This is a partial implementation of the PPO settings, and is not exhaustive
 */
USTRUCT(BlueprintType)
struct SCHOLATRAINING_API FSB3PPOSettings : public FTrainingSettings
{
	GENERATED_BODY()

public:
	/** The learning rate for the PPO algorithm */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PPO Settings")
	float LearningRate = 0.0003;

	/** The number of steps to take between training steps */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PPO Settings")
	int NSteps = 2048;

	/** The batch size to use during gradient descent */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PPO Settings")
	int BatchSize = 64;

	/** The number of epochs to train for each training step */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PPO Settings")
	int NEpochs = 10;

	/** The gamma value for the PPO algorithm */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PPO Settings")
	float Gamma = 0.99;

	/** The Generalized Advantage Estimate Lambda value for the PPO algorithm */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PPO Settings")
	float GAELambda = 0.95;

	/** The clip range for the PPO algorithm */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PPO Settings")
	float ClipRange = 0.2;

	/** Should we normalize the advantage values */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PPO Settings")
	bool NormalizeAdvantage = true;

	/** The entropy coefficient for the PPO algorithm */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PPO Settings")
	float EntCoef = 0.0;

	/** The value function coefficient for the PPO algorithm */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PPO Settings")
	float VFCoef = 0.05;

	/** The maximum gradient norm for the PPO algorithm */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PPO Settings")
	float MaxGradNorm = 0.5;

	/** Should we use state dependent entropy noise */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PPO Settings")
	bool UseSDE = false;

	/** The frequency to sample the state dependent entropy noise */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PPO Settings")
	int SDESampleFreq = -1;

	void GenerateTrainingArgs(FScriptArgBuilder& ArgBuilder) const;

	virtual ~FSB3PPOSettings();
};
