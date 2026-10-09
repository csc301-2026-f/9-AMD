// Copyright (c) 2026 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Environment/CppOnlyMultiAgentEnvironmentInterface.h"
#include "StateTreeReference.h"
#include "StateTreeTrainingEnvironment.generated.h"

class UStateTree;
class UScholaStateTreeComponent;
class UStateTreeTask_StepInference;
class UStateTreeEvaluator_RLDecision;

/**
 * @brief Multi-agent training environment with integrated StateTree.
 *
 * ## Architecture:
 * - **Environment**: Central hub that manages the training loop and agent lifecycle.
 * - **Task (UStateTreeTask_StepInference)**: Each task defines an agent's obs/action
 *   spaces and implements Observe, Act, ComputeReward. Tasks register with this
 *   environment on EnterState and unregister on ExitState.
 * - **Evaluator**: Queries environment for branch actions, drives transitions.
 *
 * ## Communication Flow:
 * 1. InitializeEnvironment starts StateTree, collects definitions from tasks/evaluators
 * 2. Python sends actions -> Environment stores them
 * 3. For each agent: Act(action) is called
 * 4. Environment ticks StateTree (transitions occur)
 * 5. For each agent: Observe(obs), ComputeReward(reward)
 * 6. Environment calls IsEpisodeOver for unified termination
 *
 * ## What the user implements:
 * - On each **Task/Evaluator** Blueprint: Define, Observe, Act, ComputeReward
 * - On the **Environment** Blueprint: IsEpisodeOver, OnEpisodeReset (optional)
 *
 * Agent definitions are auto-collected from registered tasks/evaluators.
 *
 * @note **Thread Safety**: This environment must be stepped on the game thread.
 * Set `bRunEnvironmentsInParallel=false` on the GymConnector when using this environment.
 */
UCLASS(Blueprintable, BlueprintType, meta = (DisplayName = "StateTree Training Environment"))
class SCHOLASTATETREE_API AStateTreeTrainingEnvironment : public AActor, public ICppOnlyMultiAgentEnvironment
{
	GENERATED_BODY()

public:
	AStateTreeTrainingEnvironment();

	// ========== Configuration ==========

	/** Maximum steps per episode before truncation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Schola|StateTree|Training")
	int32 MaxStepsPerEpisode = 1000;

	// ========== Runtime State ==========

	/** Current step count in the episode */
	UPROPERTY(BlueprintReadOnly, Category = "Schola|StateTree|Training")
	int32 CurrentStep = 0;

	// ========== Task Registration (called by tasks on EnterState/ExitState) ==========

	/**
	 * Register a task as an active agent. Called automatically by
	 * UStateTreeTask_StepInference::EnterState. Agent ID is derived from state name.
	 * @param Task The task instance providing Observe/Act/ComputeReward
	 */
	void RegisterTask(UStateTreeTask_StepInference* Task);

	/**
	 * Unregister a task. Called automatically by
	 * UStateTreeTask_StepInference::ExitState.
	 * @param Task The task instance to unregister
	 */
	void UnregisterTask(UStateTreeTask_StepInference* Task);

	// ========== Evaluator Registration ==========

	/**
	 * Register an evaluator as an active agent. Called automatically by
	 * UStateTreeEvaluator_RLDecision::TreeStart. Agent ID is derived from evaluator name.
	 * @param Evaluator The evaluator instance providing Observe/Act/ComputeReward
	 */
	void RegisterEvaluator(UStateTreeEvaluator_RLDecision* Evaluator);

	/**
	 * Unregister an evaluator. Called automatically by
	 * UStateTreeEvaluator_RLDecision::TreeStop.
	 * @param Evaluator The evaluator instance to unregister
	 */
	void UnregisterEvaluator(UStateTreeEvaluator_RLDecision* Evaluator);

	// ========== Agent Activation (backward compat and evaluator support) ==========

	/**
	 * Activate an agent by ID. Used by the evaluator for branch selector agents.
	 * For task-based agents, use RegisterTask instead.
	 */
	UFUNCTION(BlueprintCallable, Category = "Schola|StateTree|Training")
	void ActivateAgent(const FString& AgentId);

	/** Deactivate an agent by ID. */
	UFUNCTION(BlueprintCallable, Category = "Schola|StateTree|Training")
	void DeactivateAgent(const FString& AgentId);

	/** Check if an agent is currently active. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Schola|StateTree|Training")
	bool IsAgentActive(const FString& AgentId) const;

	// ========== Action Access ==========

	/**
	 * Get the branch action for an evaluator agent.
	 * @param EvaluatorAgentId The evaluator's agent ID
	 * @return Branch index (0-N), or -1 if no action available
	 */
	UFUNCTION(BlueprintCallable, Category = "Schola|StateTree|Training")
	int32 GetBranchAction(const FString& EvaluatorAgentId) const;

	// ========== Blueprint Events ==========

	/**
	 * Check if the episode is over (unified across all agents).
	 * Override this in Blueprint. Since all agents in a StateTree share the same
	 * simulation, termination applies to all of them equally.
	 *
	 * @param OutTerminated True if episode should end (goal reached or failure)
	 * @param OutTruncated True if episode was truncated (time limit, external)
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Schola|StateTree|Training")
	void IsEpisodeOver(bool& OutTerminated, bool& OutTruncated);

	/**
	 * Reset game state when episode resets.
	 * Override this in Blueprint for any reset logic.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Schola|StateTree|Training")
	void OnEpisodeReset();

	// ========== ICppOnlyMultiAgentEnvironment Implementation (not exposed to Blueprint) ==========

	// Bring AActor::Reset() into scope (different signature from interface Reset)
	using AActor::Reset;

	virtual void InitializeEnvironment(TMap<FString, FInteractionDefinition>& OutAgentDefinitions) override;
	virtual void SeedEnvironment(int Seed) override;
	virtual void SetEnvironmentOptions(const TMap<FString, FString>& Options) override;
	virtual void Reset(TMap<FString, FInitialAgentState>& OutAgentState) override;
	virtual void Step(const TMap<FString, FInstancedStruct>& InActions, TMap<FString, FAgentState>& OutAgentStates) override;

protected:
	/** Set of currently active agent IDs */
	UPROPERTY()
	TSet<FString> ActiveAgents;

	/** Registered task instances, keyed by agent ID */
	UPROPERTY()
	TMap<FString, TObjectPtr<UStateTreeTask_StepInference>> RegisteredTasks;

	/** Registered evaluator instances, keyed by agent ID */
	UPROPERTY()
	TMap<FString, TObjectPtr<UStateTreeEvaluator_RLDecision>> RegisteredEvaluators;

	/** Cached definitions from registered agents (captured at registration time) */
	TMap<FString, FInteractionDefinition> CachedDefinitions;

	/** Per-environment helper instances for calling ResetForEpisode on all tasks.
	 *  These are separate from runtime instances to avoid CDO mutation. */
	UPROPERTY()
	TMap<FString, TObjectPtr<UStateTreeTask_StepInference>> CachedTaskHelpers;

	/** Per-environment helper instances for calling ResetForEpisode on all evaluators.
	 *  These are separate from runtime instances to avoid CDO mutation. */
	UPROPERTY()
	TMap<FString, TObjectPtr<UStateTreeEvaluator_RLDecision>> CachedEvaluatorHelpers;

	/** Stored actions from the last Step call */
	TMap<FString, FInstancedStruct> CurrentActions;

	/** Agents that exited this step (need terminated=true for RLlib) */
	TSet<FString> PendingDoneAgents;

	/** Agents that have already been marked done in this episode - don't send more data for them */
	TSet<FString> EpisodeDoneAgents;

	/** Agents that were ever active THIS episode - used for proper termination */
	TSet<FString> AgentsActiveThisEpisode;

	/** Random stream for seeding */
	FRandomStream RandomStream;

	/** Find ScholaStateTreeComponent on this actor */
	UScholaStateTreeComponent* FindStateTreeComponent() const;
};
