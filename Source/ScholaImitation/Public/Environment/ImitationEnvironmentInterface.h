// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TrainingDataTypes/EnvironmentDefinition.h"
#include "Common/InteractionDefinition.h"
#include "TrainingDataTypes/EnvironmentUpdate.h"
#include "TrainingDataTypes/EnvironmentState.h"
#include "ImitationDataTypes/ImitationAgentState.h"
#include "Containers/Map.h"
#include "ImitationEnvironmentInterface.generated.h"

struct FInteractionDefinition;
struct FPoint;

/**
 * @brief A type-erased interface for a variety of Schola Environments (single and multi agent)
 * @details This interface provides a common abstraction for both single-agent and multi-agent
 * imitation learning environments, allowing them to be managed uniformly.
 */
class SCHOLAIMITATION_API IImitationScholaEnvironment
{
public:
	/**
	 * @brief Initialize the environment and retrieve agent definitions.
	 * @param[out] OutAgentDefinitions Map of agent names to their interaction definitions
	 */
	virtual void InitializeEnvironment(TMap<FString, FInteractionDefinition>& OutAgentDefinitions) = 0;

	/**
	 * @brief Set the random seed for the environment.
	 * @param[in] Seed The random seed value
	 */
	virtual void SeedEnvironment(int Seed) = 0;

	/**
	 * @brief Configure environment options.
	 * @param[in] Options Map of option names to their string values
	 */
	virtual void SetEnvironmentOptions(const TMap<FString, FString>& Options) = 0;

	/**
	 * @brief Reset the environment to its initial state.
	 * @param[out] OutAgentState Map of agent names to their initial states
	 */
	virtual void Reset(TMap<FString, FInitialAgentState>& OutAgentState) = 0;

	/**
	 * @brief Execute one step of the environment with expert demonstrations.
	 * @param[out] OutAgentStates Map of agent names to their imitation states (observations, actions, rewards)
	 */
	virtual void Step(TMap<FString, FImitationAgentState>& OutAgentStates) = 0;

	/**
	 * @brief Virtual destructor for proper cleanup.
	 */
	virtual ~IImitationScholaEnvironment() = default;
};

/**
 * @brief Template wrapper that adapts Blueprint-implementable environment interfaces to IImitationScholaEnvironment.
 * @tparam T The Blueprint interface type (e.g., IMultiAgentImitationScholaEnvironment)
 * @details This template bridges Blueprint-exposed environment implementations with the
 * type-erased IImitationScholaEnvironment interface for polymorphic usage.
 */
template <typename T>
class SCHOLAIMITATION_API TImitationScholaEnvironment : public TScriptInterface<T>, public IImitationScholaEnvironment
{
public:
	/**
	 * @brief Default destructor.
	 */
	~TImitationScholaEnvironment() = default;

	/**
	 * @brief Constructs the environment wrapper from a UObject implementing the interface.
	 * @param[in] InObject The UObject that implements the environment interface
	 */
	TImitationScholaEnvironment(UObject* InObject)
		: TScriptInterface<T>(InObject) {};

	void InitializeEnvironment(TMap<FString, FInteractionDefinition>& OutDefinition) override
	{
		// This should return definitions for outputs, map will go to different agents
		T::Execute_InitializeEnvironment(this->GetObject(), OutDefinition);
	};

	/**
	 * @brief Reset the environment to its initial state.
	 * @param[out] OutAgentState Map of agent names to their initial states
	 */
	void Reset(TMap<FString, FInitialAgentState>& OutAgentState) override
	{
		T::Execute_Reset(this->GetObject(), OutAgentState);
	};

	/**
	 * @brief Execute one step of the environment with expert demonstrations.
	 * @param[out] OutAgentStates Map of agent names to their imitation states
	 */
	void Step(TMap<FString, FImitationAgentState>& OutAgentStates) override
	{
		T::Execute_Step(this->GetObject(), OutAgentStates);
	};

	/**
	 * @brief Configure environment options.
	 * @param[in] Options Map of option names to their string values
	 */
	void SetEnvironmentOptions(const TMap<FString, FString>& Options) override
	{
		T::Execute_SetEnvironmentOptions(this->GetObject(), Options);
	};

	/**
	 * @brief Set the random seed for the environment.
	 * @param[in] Seed The random seed value
	 */
	void SeedEnvironment(int Seed) override
	{
		T::Execute_SeedEnvironment(this->GetObject(), Seed);
	};
};

/**
 * @brief Base UInterface for all Schola Imitation Environments.
 */
UINTERFACE(BlueprintType, Blueprintable)
class SCHOLAIMITATION_API UBaseImitationScholaEnvironment : public UInterface
{
	GENERATED_BODY()
};

/**
 * @brief Native interface for base imitation environment functionality.
 * @details Implement this interface to make your environment discoverable by the imitation connector.
 */
class SCHOLAIMITATION_API IBaseImitationScholaEnvironment
{
	GENERATED_BODY()
};
