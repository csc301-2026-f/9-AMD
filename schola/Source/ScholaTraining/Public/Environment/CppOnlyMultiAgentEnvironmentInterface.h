// Copyright (c) 2026 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Environment/EnvironmentInterface.h"
#include "TrainingDataTypes/AgentState.h"
#include "Containers/Map.h"
#include "Common/InteractionDefinition.h"
#include "Points/Point.h"
#include "CppOnlyMultiAgentEnvironmentInterface.generated.h"

/**
 * @brief C++-only interface for StateTree training environments.
 * @details This interface is NOT exposed to Blueprint, preventing users from
 * accidentally overriding the core environment methods. Use this instead of
 * IMultiAgentScholaEnvironment when you want to hide the implementation details.
 */
UINTERFACE(meta = (CannotImplementInterfaceInBlueprint))
class SCHOLATRAINING_API UCppOnlyMultiAgentEnvironment : public UBaseScholaEnvironment
{
	GENERATED_BODY()
};

/**
 * @brief Interface for StateTree-based multi-agent environments.
 * @details Pure C++ interface - methods are not exposed to Blueprint.
 * Implement this in your environment class to create a StateTree RL environment.
 * Each agent is identified by a unique string ID.
 */
class SCHOLATRAINING_API ICppOnlyMultiAgentEnvironment : public IBaseScholaEnvironment
{
	GENERATED_BODY()

public:
	/**
	 * @brief Initialize the environment and define all agents' observation and action spaces.
	 * @param[out] OutAgentDefinitions Map of agent IDs to their interaction definitions.
	 */
	virtual void InitializeEnvironment(TMap<FString, FInteractionDefinition>& OutAgentDefinitions) = 0;

	/**
	 * @brief Set the random seed for reproducible environment behavior.
	 * @param[in] Seed The random seed value.
	 */
	virtual void SeedEnvironment(int Seed) = 0;

	/**
	 * @brief Configure the environment with custom options.
	 * @param[in] Options Map of configuration option names to values.
	 */
	virtual void SetEnvironmentOptions(const TMap<FString, FString>& Options) = 0;

	/**
	 * @brief Reset the environment to its initial state for all agents.
	 * @param[out] OutAgentState Map of agent IDs to their initial states after reset.
	 */
	virtual void Reset(TMap<FString, FInitialAgentState>& OutAgentState) = 0;

	/**
	 * @brief Execute one environment step with actions from all agents.
	 * @param[in] InActions Map of agent IDs to their selected actions.
	 * @param[out] OutAgentStates Map of agent IDs to their resulting states.
	 */
	virtual void Step(const TMap<FString, FInstancedStruct>& InActions, TMap<FString, FAgentState>& OutAgentStates) = 0;

	// ========== Static Execute_ methods for TScholaEnvironment compatibility ==========

	/** Dispatches InitializeEnvironment to Obj if it implements ICppOnlyMultiAgentEnvironment. */
	static void Execute_InitializeEnvironment(UObject* Obj, TMap<FString, FInteractionDefinition>& OutAgentDefinitions)
	{
		if (ICppOnlyMultiAgentEnvironment* Env = Cast<ICppOnlyMultiAgentEnvironment>(Obj))
		{
			Env->InitializeEnvironment(OutAgentDefinitions);
		}
	}

	/** Dispatches SeedEnvironment to Obj if it implements ICppOnlyMultiAgentEnvironment. */
	static void Execute_SeedEnvironment(UObject* Obj, int Seed)
	{
		if (ICppOnlyMultiAgentEnvironment* Env = Cast<ICppOnlyMultiAgentEnvironment>(Obj))
		{
			Env->SeedEnvironment(Seed);
		}
	}

	/** Dispatches SetEnvironmentOptions to Obj if it implements ICppOnlyMultiAgentEnvironment. */
	static void Execute_SetEnvironmentOptions(UObject* Obj, const TMap<FString, FString>& Options)
	{
		if (ICppOnlyMultiAgentEnvironment* Env = Cast<ICppOnlyMultiAgentEnvironment>(Obj))
		{
			Env->SetEnvironmentOptions(Options);
		}
	}

	/** Dispatches Reset to Obj if it implements ICppOnlyMultiAgentEnvironment. */
	static void Execute_Reset(UObject* Obj, TMap<FString, FInitialAgentState>& OutAgentState)
	{
		if (ICppOnlyMultiAgentEnvironment* Env = Cast<ICppOnlyMultiAgentEnvironment>(Obj))
		{
			Env->Reset(OutAgentState);
		}
	}

	/** Dispatches Step to Obj if it implements ICppOnlyMultiAgentEnvironment (type-erased action map). */
	static void Execute_Step(UObject* Obj, const TMap<FString, TInstancedStruct<FPoint>>& InActions, TMap<FString, FAgentState>& OutAgentStates)
	{
		if (ICppOnlyMultiAgentEnvironment* Env = Cast<ICppOnlyMultiAgentEnvironment>(Obj))
		{
			const TMap<FString, FInstancedStruct>& TypeErasedActions = reinterpret_cast<const TMap<FString, FInstancedStruct>&>(InActions);
			Env->Step(TypeErasedActions, OutAgentStates);
		}
	}
};
