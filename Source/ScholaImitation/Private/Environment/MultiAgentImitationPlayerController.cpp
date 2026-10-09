// Copyright (c) 2026 Advanced Micro Devices, Inc. All Rights Reserved.

#include "Environment/MultiAgentImitationPlayerController.h"
#include "Points/DictPoint.h"
#include "Spaces/DictSpace.h"

AMultiAgentImitationPlayerController::AMultiAgentImitationPlayerController()
{
}

// ========== IMultiAgentImitationScholaEnvironment Implementation ==========

void AMultiAgentImitationPlayerController::InitializeEnvironment_Implementation(TMap<FString, FInteractionDefinition>& OutAgentDefinitions)
{
	// Default: empty - override in Blueprint to define observation and action spaces for each agent
}

void AMultiAgentImitationPlayerController::SeedEnvironment_Implementation(int Seed)
{
	RandomSeed = Seed;
	FMath::SRandInit(Seed);
}

void AMultiAgentImitationPlayerController::SetEnvironmentOptions_Implementation(const TMap<FString, FString>& Options)
{
	EnvironmentOptions = Options;
}

void AMultiAgentImitationPlayerController::Reset_Implementation(TMap<FString, FInitialAgentState>& OutAgentState)
{
	// Default: empty - override in Blueprint to reset game state and return initial observations
}

void AMultiAgentImitationPlayerController::Step_Implementation(TMap<FString, FImitationAgentState>& OutAgentStates)
{
	// Default: empty - override in Blueprint to collect observations, actions, rewards, and episode status
}

// ========== Utility Functions ==========

TArray<FString> AMultiAgentImitationPlayerController::GetAgentNames() const
{
	TArray<FString> Names;
	InputMappingContexts.GetKeys(Names);
	return Names;
}
