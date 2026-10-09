// Copyright (c) 2023 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Spaces/Space.h"
#include "InteractionDefinition.generated.h"

/**
 * @struct FInteractionDefinition
 * @brief Struct containing a definition of the inputs and outputs of a policy.
 * 
 * This structure defines the observation and action spaces for an agent,
 * specifying what observations it can receive and what actions it can produce.
 * This is fundamental to defining the interface between an agent and its environment.
 */
USTRUCT(BlueprintType)
struct SCHOLA_API FInteractionDefinition
{

	GENERATED_BODY()

	/**
	 * @brief Optional type used by training integrations to group compatible agents.
	 *
	 * Agents with the same non-empty type may be assigned to a shared policy during training.
	 * Leave empty to let training integrations fall back to the unique agent ID.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Schola|Interaction", meta = (ToolTip = "Optional type used by training integrations to group compatible agents under a shared policy. Leave empty to use the unique agent ID."))
	FString AgentType;

	/**
	 * @brief Defines the range of values that the corresponding agent accepts as observations.
	 * 
	 * This space describes the structure and valid values for observations
	 * that the agent receives from its environment.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Schola|Interaction")
	TInstancedStruct<FSpace> ObsSpaceDefn;

	/**
	 * @brief Defines the range of values that are output by this agent's policy as actions.
	 * 
	 * This space describes the structure and valid values for actions
	 * that the agent can produce and send to its environment.
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Schola|Interaction")
	TInstancedStruct<FSpace> ActionSpaceDefn;

	/**
	 * @brief Copy constructor.
	 * @param[in] Other The interaction definition to copy from.
	 */
	FInteractionDefinition(const FInteractionDefinition& Other)
	{
		AgentType = Other.AgentType;
		ObsSpaceDefn = Other.ObsSpaceDefn;
		ActionSpaceDefn = Other.ActionSpaceDefn;
	}

	/**
	 * @brief Constructor with observation and action space parameters.
	 * @param[in] InObsSpaceDefn The observation space definition.
	 * @param[in] InActionSpaceDefn The action space definition.
	 */
	FInteractionDefinition(const TInstancedStruct<FSpace>& InObsSpaceDefn, const TInstancedStruct<FSpace>& InActionSpaceDefn)
		: ObsSpaceDefn(InObsSpaceDefn), ActionSpaceDefn(InActionSpaceDefn)
	{

	}

	/**
	 * @brief Constructor with agent type, observation, and action space parameters.
	 * @param[in] InAgentType The optional policy grouping type for the agent.
	 * @param[in] InObsSpaceDefn The observation space definition.
	 * @param[in] InActionSpaceDefn The action space definition.
	 */
	FInteractionDefinition(const FString& InAgentType, const TInstancedStruct<FSpace>& InObsSpaceDefn, const TInstancedStruct<FSpace>& InActionSpaceDefn)
		: AgentType(InAgentType), ObsSpaceDefn(InObsSpaceDefn), ActionSpaceDefn(InActionSpaceDefn)
	{

	}

	/**
	 * @brief Default constructor.
	 */
	FInteractionDefinition()
	{

	}

};
