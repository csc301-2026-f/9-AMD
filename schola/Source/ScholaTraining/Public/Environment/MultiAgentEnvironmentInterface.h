// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TrainingDataTypes/AgentState.h"
#include "Containers/Map.h"
#include "Environment/EnvironmentInterface.h"
#include "Common/InteractionDefinition.h"
#include "MultiAgentEnvironmentInterface.generated.h"


/**
 * @brief Blueprint interface for multi-agent Schola environments.
 * @details Use this interface when implementing an environment with multiple agents.
 * Supports heterogeneous agents with different observation/action spaces. However, this may not be supported by upstream frameworks (e.g. SB3).
 */
UINTERFACE(BlueprintType, Blueprintable)
class SCHOLATRAINING_API UMultiAgentScholaEnvironment : public UBaseScholaEnvironment
{
	GENERATED_BODY()
};

/**
 * @brief Interface for multi-agent reinforcement learning environments.
 * @details Implement this interface in your environment class to create a multi-agent RL environment.
 * All methods are Blueprint Native Events, allowing implementation in either C++ or Blueprint.
 * Each agent is identified by a unique string ID.
 */
class SCHOLATRAINING_API IMultiAgentScholaEnvironment : public IBaseScholaEnvironment
{
	GENERATED_BODY()

public:

	/**
	 * @brief Initialize the environment and define all agents' observation and action spaces.
	 * @param[out] OutAgentDefinitions Map of agent IDs to their interaction definitions.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Schola|Environment")
	void InitializeEnvironment(TMap<FString, FInteractionDefinition>& OutAgentDefinitions);

	/**
	 * @brief Set the random seed for reproducible environment behavior.
	 * @param[in] Seed The random seed value.
	 * @details This seed is used to initialize the environment's random number generator.
	 * Only invoked when the environment is reset and a seed is provided.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Schola|Environment")
	void SeedEnvironment(int Seed);

	/**
	 * @brief Configure the environment with custom options.
	 * @param[in] Options Map of configuration option names to values.
	 * @details These options are specific to the environment implementation and are used to configure the environment. 
	 * Only invoked when the environment is reset and Options are provided.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Schola|Environment")
	void SetEnvironmentOptions(const TMap<FString, FString>& Options);

	/**
	 * @brief Reset the environment to its initial state for all agents.
	 * @param[out] OutAgentState Map of agent IDs to their initial states after reset.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Schola|Environment")
	void Reset(TMap<FString, FInitialAgentState>& OutAgentState);
	
	/**
	 * @brief Execute one environment step with actions from all agents.
	 * @param[in] InActions Map of agent IDs to their selected actions.
	 * @param[out] OutAgentStates Map of agent IDs to their resulting states.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Schola|Environment")
	void Step(const TMap<FString, FInstancedStruct>& InActions, TMap<FString, FAgentState>& OutAgentStates);

	/**
	 * @brief Type-safe adapter method for executing steps with typed actions.
	 * @param[in] Obj The environment object.
	 * @param[in] InActions Map of agent IDs to their typed actions.
	 * @param[out] OutAgentStates Map of agent IDs to their resulting states.
	 */
	static void Execute_Step(UObject* Obj, const TMap < FString, TInstancedStruct<FPoint>>& InActions, TMap<FString, FAgentState>& OutAgentStates)
	{
		const TMap<FString, FInstancedStruct>& TypeErasedActions = reinterpret_cast<const TMap<FString, FInstancedStruct>&>(InActions);
		IMultiAgentScholaEnvironment::Execute_Step(Obj, TypeErasedActions, OutAgentStates);
	}
};
