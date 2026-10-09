// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TrainingDataTypes/AgentState.h"
#include "Containers/Map.h"
#include "Environment/ImitationEnvironmentInterface.h"
#include "Common/InteractionDefinition.h"
#include "ImitationDataTypes/ImitationAgentState.h"
#include "MultiAgentImitationEnvironmentInterface.generated.h"

/**
 * @brief UInterface for multi-agent imitation learning environments.
 * @details Implement this interface in Blueprint or C++ to create environments
 * with multiple agents that learn from expert demonstrations.
 */
UINTERFACE(BlueprintType, Blueprintable)
class SCHOLAIMITATION_API UMultiAgentImitationScholaEnvironment : public UBaseImitationScholaEnvironment
{
	GENERATED_BODY()
};

/**
 * @brief Native interface for multi-agent imitation learning environments.
 * @details Provides the contract for environments that manage multiple agents,
 * each potentially with different observation and action spaces.
 */
class SCHOLAIMITATION_API IMultiAgentImitationScholaEnvironment : public IBaseImitationScholaEnvironment
{
	GENERATED_BODY()

public:
	/**
	 * @brief Initialize the environment and define all agents and their interaction spaces.
	 * @param[out] OutAgentDefinitions Map from agent names to their interaction definitions (observation/action spaces)
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Schola|Environment")
	void InitializeEnvironment(TMap<FString, FInteractionDefinition>& OutAgentDefinitions);

	/**
	 * @brief Set the random seed for reproducible environment behavior.
	 * @param[in] Seed The random seed value
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Schola|Environment")
	void SeedEnvironment(int Seed);

	/**
	 * @brief Configure environment-specific options.
	 * @param[in] Options Map of option names to their string values
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Schola|Environment")
	void SetEnvironmentOptions(const TMap<FString, FString>& Options);

	/**
	 * @brief Reset the environment and return initial states for all agents.
	 * @param[out] OutAgentState Map from agent names to their initial states
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Schola|Environment")
	void Reset(TMap<FString, FInitialAgentState>& OutAgentState);

	/**
	 * @brief Execute one environment step and return imitation states for all agents.
	 * @param[out] OutAgentStates Map from agent names to their imitation states (observations, expert actions, rewards)
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Schola|Environment")
	void Step(TMap<FString, FImitationAgentState>& OutAgentStates);
};
