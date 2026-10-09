// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Policies/PolicyInterface.h"
#include "Agent/AgentInterface.h"
#include "StepperInterface.generated.h"

/**
 * @brief UInterface for stepper implementations.
 * 
 * This interface allows Blueprint and C++ implementations of steppers that manage
 * the interaction loop between agents and policies.
 */
UINTERFACE(Blueprintable)
class SCHOLAINFERENCEUTILS_API UStepper : public UInterface
{
    GENERATED_BODY()
};

/**
 * @brief Interface for stepper implementations.
 * 
 * A stepper manages the observation-action loop by coordinating agents and policies.
 * It collects observations from agents, passes them to a policy for inference, and
 * applies the resulting actions back to the agents.
 */
class SCHOLAINFERENCEUTILS_API IStepper
{
    GENERATED_BODY()

public:

    /**
     * @brief Initialize the stepper with a single agent and policy.
     * 
     * Convenience method that wraps the agent in an array and calls the multi-agent Init.
     * 
     * @param[in] InAgent The agent to use for stepping
     * @param[in] InPolicy The policy to use for inference
     * @return true if initialization succeeded, false otherwise
     */
    virtual bool Init(const TScriptInterface<IAgent>& InAgent, const TScriptInterface<IPolicy>& InPolicy)
    {
        // Initialize the stepper with a single agent and policy
        TArray<TScriptInterface<IAgent>> Agents = {InAgent};
        return this->Init(Agents, InPolicy);
    }

    /**
     * @brief Initialize the stepper with multiple agents and a policy.
     * 
     * Sets up the stepper to manage multiple agents using a single shared policy.
     * The policy must support batched inference for multiple agents.
     * 
     * @param[in] InAgents Array of agents to manage
     * @param[in] InPolicy The policy to use for inference
     * @return true if initialization succeeded, false otherwise
     */
    virtual bool Init(const TArray<TScriptInterface<IAgent>>& InAgents, const TScriptInterface<IPolicy>& InPolicy) = 0;

};


