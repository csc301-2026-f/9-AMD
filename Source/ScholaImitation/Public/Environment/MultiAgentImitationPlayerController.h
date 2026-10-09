// Copyright (c) 2026 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Environment/ImitationPlayerControllerBase.h"
#include "Environment/MultiAgentImitationEnvironmentInterface.h"
#include "MultiAgentImitationPlayerController.generated.h"

/**
 * @brief A Blueprint-extendable PlayerController that implements the multi-agent imitation interface.
 * @details This class provides a convenient base for creating human demonstration collection scenarios
 * with multiple agents. It includes helper functions for Enhanced Input integration.
 *
 * To use:
 * 1. Create a Blueprint subclass of this controller
 * 2. Override the interface methods to define observation/action spaces and implement environment behavior
 * 3. Optionally set the InputMappingContexts map if using the Enhanced Input helpers
 */
UCLASS(Blueprintable, BlueprintType)
class SCHOLAIMITATION_API AMultiAgentImitationPlayerController : public AImitationPlayerControllerBase, public IMultiAgentImitationScholaEnvironment
{
	GENERATED_BODY()

public:
	AMultiAgentImitationPlayerController();

	// Bring base class Reset into scope to prevent hiding
	using APlayerController::Reset;

	// ========== Configuration ==========

	/**
	 * @brief Map of agent names to their Input Mapping Contexts.
	 * @details Set this if you want to use BuildActionSpaceFromIMC() and PollActionsFromIMC() helpers.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Schola|Imitation")
	TMap<FString, TObjectPtr<UInputMappingContext>> InputMappingContexts;

	// ========== IMultiAgentImitationScholaEnvironment Implementation ==========

	/**
	 * @brief Initialize the environment and define all agents' interaction spaces.
	 * @details Override this to define observation and action spaces for each agent.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Schola|Environment")
	void InitializeEnvironment(TMap<FString, FInteractionDefinition>& OutAgentDefinitions);

	/**
	 * @brief Set the random seed for reproducible environment behavior.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Schola|Environment")
	void SeedEnvironment(int Seed);

	/**
	 * @brief Configure environment-specific options.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Schola|Environment")
	void SetEnvironmentOptions(const TMap<FString, FString>& Options);

	/**
	 * @brief Reset the environment and return the agents' initial states.
	 * @details Override this to reset your game state and return initial observations for each agent.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Schola|Environment")
	void Reset(TMap<FString, FInitialAgentState>& OutAgentState);

	/**
	 * @brief Execute one environment step and return the agents' imitation states.
	 * @details Override this to collect observations, actions, rewards, and episode status for each agent.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Schola|Environment")
	void Step(TMap<FString, FImitationAgentState>& OutAgentStates);

	// ========== Utility Functions ==========

	/**
	 * @brief Get the list of agent names registered in this environment.
	 * @return Array of agent names from InputMappingContexts
	 */
	UFUNCTION(BlueprintCallable, Category = "Schola|Imitation")
	TArray<FString> GetAgentNames() const;

protected:
	/** Random seed for reproducibility */
	int32 RandomSeed = 0;

	/** Environment options */
	TMap<FString, FString> EnvironmentOptions;
};
