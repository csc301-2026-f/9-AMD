// Copyright (c) 2026 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Environment/ImitationPlayerControllerBase.h"
#include "Environment/SingleAgentImitationEnvironmentInterface.h"
#include "Common/InstancedStructUtils.h"
#include "ImitationPlayerController.generated.h"

/**
 * @brief A Blueprint-extendable PlayerController that implements the single-agent imitation interface.
 * @details This class provides a convenient base for creating human demonstration collection scenarios.
 * It includes helper functions for Enhanced Input integration.
 *
 * To use:
 * 1. Create a Blueprint subclass of this controller
 * 2. Set the InputMappingContext property to your IMC asset (action space is built automatically from it)
 * 3. Override DefineObservationSpace to define the observation space
 * 4. Override Reset and Step to implement environment behavior
 */
UCLASS(Blueprintable, BlueprintType)
class SCHOLAIMITATION_API AImitationPlayerController : public AImitationPlayerControllerBase, public ISingleAgentImitationScholaEnvironment
{
	GENERATED_BODY()

public:
	AImitationPlayerController();

	// Bring base class Reset into scope to prevent hiding
	using APlayerController::Reset;

	// ========== Configuration ==========

	/**
	 * @brief The Input Mapping Context used for automatic action space building.
	 * @details Set this to your IMC asset. The action space will be automatically built from it
	 * during InitializeEnvironment. Also used by PollActionsFromIMC() to poll current input values.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Schola|Imitation")
	TObjectPtr<UInputMappingContext> InputMappingContext;

	// ========== User Override Points ==========

	/**
	 * @brief Define the observation space for this environment.
	 * @details Override this in Blueprint to specify what observations your agent receives.
	 * The action space is automatically built from the InputMappingContext.
	 * @param[out] OutObservationSpace The observation space definition to populate.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Schola|Environment")
	void DefineObservationSpace(FInstancedStruct& OutObservationSpace);

	/**
	 * @brief Type-safe wrapper for DefineObservationSpace.
	 * @param[out] OutObservationSpace The observation space as TInstancedStruct<FSpace>
	 */
	void DefineObservationSpaceTyped(TInstancedStruct<FSpace>& OutObservationSpace)
	{
		DefineObservationSpace(ToUntypedInstancedStruct(OutObservationSpace));
	}

	// ========== ISingleAgentImitationScholaEnvironment Implementation ==========

	/**
	 * @brief Initialize the environment and define the agent's interaction space.
	 * @details By default, calls DefineObservationSpace for observations and builds
	 * the action space automatically from InputMappingContext.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Schola|Environment")
	void InitializeEnvironment(FInteractionDefinition& OutAgentDefinition);

	/**
	 * @brief Set the random seed for reproducible environment behavior.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Schola|Environment")
	void SeedEnvironment(int InSeed);

	/**
	 * @brief Configure environment-specific options.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Schola|Environment")
	void SetEnvironmentOptions(const TMap<FString, FString>& InOptions);

	/**
	 * @brief Reset the environment and return the agent's initial state.
	 * @details Override this to reset your game state and return the initial observation.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Schola|Environment")
	void Reset(FInitialAgentState& OutAgentState);

	/**
	 * @brief Execute one environment step and return the agent's imitation state.
	 * @details Override this to collect observations, actions, rewards, and episode status.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Schola|Environment")
	void Step(FImitationAgentState& OutAgentState);
};
