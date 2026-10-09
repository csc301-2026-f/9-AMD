// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"

#include "TrainingUtils/ArgBuilder.h"
#include "TrainingSettings/TrainingSettings.h"
#include "RLlibSACSettings.generated.h"

/**
 * @brief A struct to hold Soft Actor-Critic (SAC) settings for an RLlib training script.
 * @note Mirrors schola.scripts.rllib.settings.SACSettings (tau, target_entropy, initial_alpha, n_step, twin_q).
 */
USTRUCT(BlueprintType)
struct SCHOLATRAINING_API FRLlibSACSettings : public FTrainingSettings
{
	GENERATED_BODY()

public:
	/** Soft update coefficient for target networks (0.0-1.0). Lower values mean slower updates. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SAC Settings", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float Tau = 0.005f;

	/** Target entropy for automatic temperature tuning. Use "auto" for automatic, or a numeric string for manual. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SAC Settings")
	FString TargetEntropy = TEXT("auto");

	/** Initial temperature/alpha for entropy regularization. Higher values encourage more exploration. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SAC Settings", meta = (ClampMin = "0.0"))
	float InitialAlpha = 1.0f;

	/** Number of steps for n-step returns. n>1 can help with credit assignment in sparse reward environments. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SAC Settings", meta = (ClampMin = "1"))
	int NStep = 1;

	/** Whether to use twin Q networks (double Q-learning) to reduce overestimation bias. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SAC Settings")
	bool bTwinQ = true;

	void GenerateTrainingArgs(FScriptArgBuilder& ArgBuilder) const;

	virtual ~FRLlibSACSettings();
};
