// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "TrainingSettings/TrainingSettings.h"
#include "TrainingUtils/ArgBuilder.h"
#include "SB3NetworkArchitectureSettings.generated.h"

/**
 * @brief An enumeration of activation functions neural networks
 */
UENUM(BlueprintType)
enum class ESB3ActivationFunctionEnum : uint8
{
	ReLU	UMETA(DisplayName = "ReLU"),
	Sigmoid UMETA(DisplayName = "Sigmoid"),
	TanH	UMETA(DisplayName = "TanH")
};


/**
 * @brief A struct to hold network architecture settings for an SB3 training script
 */
USTRUCT(BlueprintType)
struct SCHOLATRAINING_API FSB3NetworkArchSettings : public FTrainingSettings
{
	GENERATED_BODY()

public:

	/** The activation function to use in the neural network */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Architecture Settings")
	ESB3ActivationFunctionEnum ActivationFunction = ESB3ActivationFunctionEnum::ReLU;

	/** The number and width of hidden layers in the Critic. Applied to either the Q-Function or Value-Function */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 0), Category = "Architecture Settings")
	TArray<int> CriticParameters = { 256, 256 };

	/** The number and width of hiddent layers in the policy network */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 0), Category = "Architecture Settings")
	TArray<int> PolicyParameters = { 256, 256 };

	void GenerateTrainingArgs(FScriptArgBuilder& ArgBuilder) const;

	virtual ~FSB3NetworkArchSettings();
};