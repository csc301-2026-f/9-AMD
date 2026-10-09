// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"

#include "TrainingUtils/ArgBuilder.h"
#include "TrainingSettings/TrainingSettings.h"
#include "TrainingSettings/Ray/RLlibLoggingSettings.h"
#include "TrainingSettings/Ray/RLlibCheckpointSettings.h"
#include "TrainingSettings/Ray/RLlibResumeSettings.h"
#include "TrainingSettings/Ray/RLlibNetworkArchitectureSettings.h"
#include "TrainingSettings/Ray/RLlibResourceSettings.h"
#include "TrainingSettings/Ray/Algorithms/RLlibPPOSettings.h"
#include "TrainingSettings/Ray/Algorithms/RLlibAPPOSettings.h"
#include "TrainingSettings/Ray/Algorithms/RLlibIMPALASettings.h"
#include "TrainingSettings/Ray/Algorithms/RLlibSACSettings.h"

#include "RLlibTrainingSettings.generated.h"


/**
 * @brief Enumeration of reinforcement learning algorithms supported by Ray RLlib.
 */
UENUM()
enum class ERLlibTrainingAlgorithm
{
	/** Proximal Policy Optimization (synchronous). */
	PPO,
	/** Asynchronous Proximal Policy Optimization. */
	APPO,
	/** Importance Weighted Actor-Learner Architecture. */
	IMPALA,
	/** Soft Actor-Critic */
	SAC
};


/**
 * @brief Configuration struct for Ray RLlib training scripts.
 * @details Contains all settings necessary to launch and configure an RLlib training session,
 * including algorithm selection, hyperparameters, logging, checkpointing, and resource allocation.
 */
USTRUCT(BlueprintType)
struct SCHOLATRAINING_API FRLlibTrainingSettings : public FTrainingSettings
{
	GENERATED_BODY()

public:
	/** The total number of environment timesteps to train for. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Training Settings")
	int Timesteps = 8000;

	/** The learning rate for the optimizer. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Training Settings")
	float LearningRate = 0.0003;

	/** The size of minibatches for gradient updates. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Training Settings")
	int MinibatchSize = 128;

	/** The training batch size per learner worker. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Training Settings")
	int TrainBatchSizePerLearner = 256;

	/** The number of SGD (stochastic gradient descent) iterations per training batch. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Training Settings")
	int NumSGDIter = 5;

	/** The discount factor (gamma) for future rewards. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Training Settings")
	float Gamma = 0.99;

	/** The logging settings for the training script */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Logging Settings")
	FRLlibLoggingSettings LoggingSettings;

	/** The checkpoint settings for the training script */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Checkpoint Settings")
	FRLlibCheckpointSettings CheckpointSettings;

	/** The resume settings for the training script */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resume Settings")
	FRLlibResumeSettings ResumeSettings;

	/** The network architecture settings for the training script */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Network Architecture Settings")
	FRLlibNetworkArchSettings NetworkArchitectureSettings;

	/** The resource settings for the training script */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resource Settings")
	FRLlibResourceSettings ResourceSettings;

	/** The algorithm to use during training (e.g. SAC, PPO) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Algorithm Settings")
	ERLlibTrainingAlgorithm Algorithm = ERLlibTrainingAlgorithm::PPO;

	/** PPO specific settings */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "Algorithm==ERLlibTrainingAlgorithm::PPO", EditConditionHides, DisplayName = "PPO Algorithm Settings"), Category = "Algorithm Settings")
	FRLlibPPOSettings PPOSettings;

	/** APPO specific settings */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "Algorithm==ERLlibTrainingAlgorithm::APPO", EditConditionHides, DisplayName = "APPO Algorithm Settings"), Category = "Algorithm Settings")
	FRLlibAPPOSettings APPOSettings;

	/** IMPALA specific settings */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "Algorithm==ERLlibTrainingAlgorithm::IMPALA", EditConditionHides, DisplayName = "IMPALA Algorithm Settings"), Category = "Algorithm Settings")
	FRLlibIMPALASettings IMPALASettings;

	/** SAC specific settings */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "Algorithm==ERLlibTrainingAlgorithm::SAC", EditConditionHides, DisplayName = "SAC Algorithm Settings"), Category = "Algorithm Settings")
	FRLlibSACSettings SACSettings;

	/**
	 * @brief Generate command-line arguments for the RLlib training script.
	 * @param[in,out] ArgBuilder The argument builder to populate with settings.
	 */
	void GenerateTrainingArgs( FScriptArgBuilder& ArgBuilder) const;

	/**
	 * @brief Virtual destructor.
	 */
	virtual ~FRLlibTrainingSettings();
};
