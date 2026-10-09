// Copyright (c) 2026 Advanced Micro Devices, Inc. All Rights Reserved.

#include "Environment/ImitationPlayerController.h"
#include "Points/DictPoint.h"
#include "Spaces/DictSpace.h"

AImitationPlayerController::AImitationPlayerController()
{
	InputMappingContext = nullptr;
}

// ========== User Override Points ==========

void AImitationPlayerController::DefineObservationSpace_Implementation(FInstancedStruct& OutObservationSpace)
{
	// Default: empty - override in Blueprint to define the observation space
}

// ========== ISingleAgentImitationScholaEnvironment Implementation ==========

void AImitationPlayerController::InitializeEnvironment_Implementation(FInteractionDefinition& OutAgentDefinition)
{
	// Let user define the observation space
	DefineObservationSpaceTyped(OutAgentDefinition.ObsSpaceDefn);

	// Automatically build action space from the Input Mapping Context
	if (InputMappingContext)
	{
		BuildActionSpaceFromIMC(InputMappingContext, OutAgentDefinition.ActionSpaceDefn);
	}
}

void AImitationPlayerController::SeedEnvironment_Implementation(int InSeed)
{
}

void AImitationPlayerController::SetEnvironmentOptions_Implementation(const TMap<FString, FString>& InOptions)
{
}

void AImitationPlayerController::Reset_Implementation(FInitialAgentState& OutAgentState)
{
	// Default: empty - override in Blueprint to reset game state and return initial observation
}

void AImitationPlayerController::Step_Implementation(FImitationAgentState& OutAgentState)
{
	// Default: empty - override in Blueprint to collect observations, actions, rewards, and episode status
}
