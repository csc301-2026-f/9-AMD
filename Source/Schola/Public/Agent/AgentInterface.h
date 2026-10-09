// Copyright (c) 2023-2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "StructUtils/InstancedStruct.h"
#include "Points/Point.h"
#include "Common/InteractionDefinition.h"
#include "Common/InstancedStructUtils.h"

#include "AgentInterface.generated.h"
/**
 * @brief The Agent State as represented in Schola. In this case, Stopped means the agent is not taking new instructions.
 */
UENUM(BlueprintType)
enum class EAgentStatus : uint8
{
	Running UMETA(DisplayName = "Running"),
	Stopped UMETA(DisplayName = "Stopped"),
	Error	UMETA(DisplayName = "Error")
};

/**
 * @brief An interface implemented by classes that represent an inference agent.
 */
UINTERFACE(Blueprintable, BlueprintType)
class SCHOLA_API UAgent : public UInterface
{
	GENERATED_BODY()
};

/**
 * @class IAgent
 * @brief Interface for inference agents in the Schola framework.
 *
 * This interface defines the core functionality that all agents must implement,
 * including status management, action space definition, action execution, and
 * observation gathering.
 */
class SCHOLA_API IAgent
{
	GENERATED_BODY()

public:
	/**
	 * @brief Gets the current status of the agent.
	 * @return The current agent status (Running, Stopped, or Error).
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Schola|Agent")
	EAgentStatus GetStatus();

	/**
	 * @brief Sets the status of the agent.
	 * @param NewStatus The new status to set for the agent.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Schola|Agent")
	void SetStatus(EAgentStatus NewStatus);

	/**
	 * @brief Defines the observation and action spaces for this agent.
	 *
	 * This method should populate the output parameter with the observation
	 * and action space definitions that describe how this agent interacts
	 * with its environment.
	 *
	 * @param[out] OutInteractionDefinition The interaction definition for this agent.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Schola|Agent")
	void Define(FInteractionDefinition& OutInteractionDefinition);

	/**
	 * @brief Executes an action provided to the agent.
	 *
	 * This method takes an action from the action space and executes it,
	 * causing the agent to perform the corresponding behavior.
	 *
	 * @param[in] InAction The action to execute, represented as an instanced struct.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Schola|Agent")
	void Act(const FInstancedStruct& InAction);

	/**
	 * @brief Gathers the current observations from the agent's environment.
	 *
	 * This method populates the output parameter with the current state
	 * observations from the agent's environment.
	 *
	 * @param[out] OutObservations The current observations from the agent's perspective.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Schola|Agent")
	void Observe(FInstancedStruct& OutObservations);

	// Typed API

	/**
	 * @brief Type-safe wrapper for executing an action with a strongly-typed Point.
	 * @param[in] Obj The object implementing the IAgent interface.
	 * @param[in] InAction The action to execute, as a strongly-typed Point instance.
	 */
	static void Execute_Act(UObject* Obj, const TInstancedStruct<FPoint>& InAction)
	{
		IAgent::Execute_Act(Obj, ToUntypedInstancedStruct(InAction));
	};

	/**
	 * @brief Type-safe wrapper for gathering observations with a strongly-typed Point.
	 * @param[in] Obj The object implementing the IAgent interface.
	 * @param[out] OutObservations The observations as a strongly-typed Point.
	 */
	static void Execute_Observe(UObject* Obj, TInstancedStruct<FPoint>& OutObservations)
	{
		IAgent::Execute_Observe(Obj, ToUntypedInstancedStruct(OutObservations));
	};
};
