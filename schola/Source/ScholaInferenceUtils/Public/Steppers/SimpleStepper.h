// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

# pragma once
#include "CoreMinimal.h"
#include "Steppers/StepperInterface.h"
#include "Agent/AgentInterface.h"
#include "LogScholaInferenceUtils.h"
#include "SimpleStepper.generated.h"

/**
 * @brief Simple synchronous stepper implementation.
 * 
 * A straightforward stepper that performs the full observation-inference-action loop
 * synchronously on the calling thread. On each Step() call:
 * 1. Collects observations from all agents
 * 2. Performs batched inference using the policy
 * 3. Applies the resulting actions to the agents
 * 
 * This stepper blocks during policy inference and is suitable for simple scenarios
 * or policies with fast inference times.
 */
UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class SCHOLAINFERENCEUTILS_API USimpleStepper : public UObject, public IStepper
{
	GENERATED_BODY()

public:
	/** Agents stepped each tick in array order. */
	UPROPERTY(BlueprintReadOnly, Category = "Stepper")
	TArray<TScriptInterface<IAgent>> Agents;

	/** Policy used for inference in Step(). */
	UPROPERTY(BlueprintReadOnly, Category = "Stepper")
	TScriptInterface<IPolicy>		 Policy;

	/**
	 * @brief Initialize the stepper with the given agents and policy.
	 * 
	 * @param[in] InAgents The array of agents to manage
	 * @param[in] InPolicy The policy to use for inference
	 * @return true if initialization succeeded, false otherwise
	 */
	UFUNCTION(BlueprintCallable, Category = "Schola|Stepper")
	bool Init(const TArray<TScriptInterface<IAgent>>& InAgents, const TScriptInterface<IPolicy>& InPolicy) override
	{	
		this->Agents = InAgents;
		this->Policy = InPolicy;

		if(this->Agents.Num() == 0)
		{
			UE_LOGFMT(LogScholaInferenceUtils, Error, "USimpleStepper::Init(): Initialized with no agents!");
			return false;
		}

		if (!this->Policy)
		{
			UE_LOGFMT(LogScholaInferenceUtils, Error, "USimpleStepper::Init(): Initialized with no policy!");
			return false;
		}

		return true;
	}

	/**
	 * @brief Execute one step of the agent-policy loop.
	 * 
	 * Performs the full observation-inference-action cycle synchronously:
	 * - Collects observations from all agents
	 * - Calls the policy's BatchedThink method
	 * - Applies the resulting actions to each agent
	 * 
	 * This method blocks during policy inference.
	 */
	UFUNCTION(BlueprintCallable, Category = "Schola|Stepper")
	void Step()
	{
		if (this->Agents.Num() == 0)
		{
			UE_LOGFMT(LogScholaInferenceUtils, Error, "USimpleStepper::Step(): No agents to step!");
			return;
		}
		if (!this->Policy)
		{
			UE_LOGFMT(LogScholaInferenceUtils, Error, "USimpleStepper::Step(): No policy to use for stepping!");
			return;
		}

		TArray<TInstancedStruct<FPoint>> Observations;
		TArray<TInstancedStruct<FPoint>> Actions;
		for (int i = 0; i < Agents.Num(); i++)
		{
			TInstancedStruct<FPoint> Observation;
			IAgent::Execute_Observe(Agents[i].GetObject(), Observation);
			Observations.Add(Observation);
		}
		if (this->Policy->BatchedThink(Observations, Actions))
		{
			if (Actions.Num() == this->Agents.Num())
			{
				for (int i = 0; i < Agents.Num(); i++)
				{
					IAgent::Execute_Act(Agents[i].GetObject(), Actions[i]);
				}
			}
			else
			{
				UE_LOGFMT(LogScholaInferenceUtils, Error, "USimpleStepper::Step(): Number of actions ({0}) does not match number of agents ({1})!", Actions.Num(), Agents.Num());
			}
		}
		else
		{
			UE_LOGFMT(LogScholaInferenceUtils, Error, "USimpleStepper::Step(): Policy failed to think!");
		}
			
		
	}
};