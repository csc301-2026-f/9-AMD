// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Environment/MultiAgentEnvironmentInterface.h"
#include "Environment/SingleAgentEnvironmentInterface.h"
#include "GameFramework/Actor.h"
#include "Components/SceneComponent.h"
#include "Spaces/MultiDiscreteSpace.h"
#include "BasicTestEnvironment.generated.h"

UCLASS(Abstract)
class ABasicTestEnvironment : public AActor
{
	GENERATED_BODY()

public:

	// Expose logical position for tests (world position might differ if movement fails)
	float GetLogicalPositionX() const { return PositionX; }

	float PositionX = 0.0f;

	const float MinX = -500.f;
	const float MaxX = 500.f;
	const float StepDelta = 100.f;

	void BuildObservation(TInstancedStruct<FPoint>& OutObservation) const;
	UPROPERTY()
	FString AgentName = TEXT("BasicAgent");
};

UCLASS()
class ABasicTestMultiAgentEnvironment : public ABasicTestEnvironment, public IMultiAgentScholaEnvironment
{
	GENERATED_BODY()
public:

	ABasicTestMultiAgentEnvironment();

	// IScholaEnvironment overrides
	void InitializeEnvironment_Implementation(TMap<FString, FInteractionDefinition>& OutAgentDefinitions) override;

	void SeedEnvironment_Implementation(int Seed) override {};

	void SetEnvironmentOptions_Implementation(const TMap<FString, FString>& InOptions) override {};

	void Reset_Implementation(TMap<FString, FInitialAgentState>& OutAgentState) override;

	void Step_Implementation(const TMap<FString, FInstancedStruct>& InActions, TMap<FString, FAgentState>& OutAgentStates) override;

};


UCLASS()
class ABasicTestSingleAgentEnvironment : public ABasicTestEnvironment, public ISingleAgentScholaEnvironment
{
	GENERATED_BODY()
public:

	ABasicTestSingleAgentEnvironment();


	// IScholaEnvironment overrides
	void InitializeEnvironment_Implementation(FInteractionDefinition& OutAgentDefinitions) override;

	void SeedEnvironment_Implementation(int Seed) override {}

	void SetEnvironmentOptions_Implementation(const TMap<FString, FString>& InOptions) override {}

	void Reset_Implementation(FInitialAgentState& OutAgentState) override;

	void Step_Implementation(const FInstancedStruct& InAction, FAgentState& OutAgentState) override;

};