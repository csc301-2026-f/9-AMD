// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "BasicTestEnvironment.h"

#include "Spaces/DictSpace.h"
#include "Spaces/BoxSpace.h"
#include "Spaces/DiscreteSpace.h"
#include "Spaces/MultiDiscreteSpace.h"
#include "Points/DictPoint.h"
#include "Points/BoxPoint.h"
#include "Points/DiscretePoint.h"
#include "Common/LogSchola.h"


ABasicTestMultiAgentEnvironment::ABasicTestMultiAgentEnvironment()
{
	PrimaryActorTick.bCanEverTick = false;
	
	if (!RootComponent)
	{
		USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
		RootComponent = SceneRoot;
	}
}

void ABasicTestEnvironment::BuildObservation(TInstancedStruct<FPoint>& OutObservation) const
{
	OutObservation.InitializeAs<FBoxPoint>();
	FBoxPoint& Box = OutObservation.GetMutable<FBoxPoint>();
	Box.Values = { PositionX };
}


void ABasicTestMultiAgentEnvironment::InitializeEnvironment_Implementation(TMap<FString, FInteractionDefinition>& OutAgentDefinitions)
{
	FInteractionDefinition Defn;

	TInstancedStruct<FSpace>& BoxSpaceInst = Defn.ObsSpaceDefn;
	BoxSpaceInst.InitializeAs<FBoxSpace>();
	FBoxSpace& Box = BoxSpaceInst.GetMutable<FBoxSpace>();
	Box.Add(-500.f, 500.f);

	TInstancedStruct<FSpace>& DiscreteSpaceInst = Defn.ActionSpaceDefn;
	DiscreteSpaceInst.InitializeAs<FMultiDiscreteSpace>();
	FMultiDiscreteSpace& Discrete = DiscreteSpaceInst.GetMutable<FMultiDiscreteSpace>();
	Discrete.Add(3);

	OutAgentDefinitions.Add(AgentName, Defn);
}

void ABasicTestMultiAgentEnvironment::Reset_Implementation(TMap<FString, FInitialAgentState>& OutAgentState)
{
	PositionX = 0.0f;
	FInitialAgentState& AgentState = OutAgentState.Add(AgentName);
	BuildObservation(AgentState.Observations);
}

void ABasicTestMultiAgentEnvironment::Step_Implementation(const TMap<FString, FInstancedStruct>& InActions, TMap<FString, FAgentState>& OutAgentStates)
{
	if (const FInstancedStruct* ActionPtr = InActions.Find(AgentName))
	{
		if (ActionPtr->IsValid())
		{
			const FDiscretePoint& Action = ActionPtr->Get<FDiscretePoint>();
			if (Action.Value == 1) { PositionX += StepDelta; }
			else if (Action.Value == 2) { PositionX -= StepDelta; }
		}
	}

	FAgentState& AgentState = OutAgentStates.Add(AgentName);

	bool bTerminated = false;
	if (PositionX >= MaxX)
	{
		PositionX = MaxX;
		AgentState.Reward = 1.0f;
		AgentState.bTerminated = true;
		bTerminated = true;
	}
	else if (PositionX <= MinX)
	{
		PositionX = MinX;
		AgentState.Reward = 0.1f;
		AgentState.bTerminated=true;
		bTerminated = true;
	}
	else
	{
		AgentState.Reward = -0.01f;
		AgentState.bTerminated = false;
		AgentState.bTruncated = false;
	}

	BuildObservation(AgentState.Observations);

	// Reflect logical position in world transform for easier external verification
	FVector Location = GetActorLocation();
	Location.X = PositionX;
	SetActorLocation(Location);
}

//Single Agent Version of the Environment

ABasicTestSingleAgentEnvironment::ABasicTestSingleAgentEnvironment()
{
	this->AgentName = TEXT("SingleAgent");

	if (!RootComponent)
	{
		USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
		RootComponent = SceneRoot;
	}
}

void ABasicTestSingleAgentEnvironment::InitializeEnvironment_Implementation(FInteractionDefinition& OutAgentDefinition)
{

	TInstancedStruct<FSpace>& BoxSpaceInst = OutAgentDefinition.ObsSpaceDefn;
	BoxSpaceInst.InitializeAs<FBoxSpace>();
	FBoxSpace& Box = BoxSpaceInst.GetMutable<FBoxSpace>();
	Box.Add(-500.f, 500.f);

	TInstancedStruct<FSpace>& DiscreteSpaceInst = OutAgentDefinition.ActionSpaceDefn;
	DiscreteSpaceInst.InitializeAs<FMultiDiscreteSpace>();
	FMultiDiscreteSpace& Discrete = DiscreteSpaceInst.GetMutable<FMultiDiscreteSpace>();
	Discrete.Add(3);

}

void ABasicTestSingleAgentEnvironment::Reset_Implementation(FInitialAgentState& OutAgentState)
{
	PositionX = 0.0f;
	BuildObservation(OutAgentState.Observations);
}

void ABasicTestSingleAgentEnvironment::Step_Implementation(const FInstancedStruct& InActions, FAgentState& OutAgentState)
{
	if (InActions.IsValid())
	{
		const FDiscretePoint& Action = InActions.Get<FDiscretePoint>();
		if (Action.Value == 1)
		{
			PositionX += StepDelta;
		}
		else if (Action.Value == 2)
		{
			PositionX -= StepDelta;
		}
	}
	
	bool bTerminated = false;
	if (PositionX >= MaxX)
	{
		PositionX = MaxX;
		OutAgentState.Reward = 1.0f;
		OutAgentState.bTerminated = true;
		bTerminated = true;
	}
	else if (PositionX <= MinX)
	{
		PositionX = MinX;
		OutAgentState.Reward = 0.1f;
		OutAgentState.bTerminated = true;
		bTerminated = true;
	}
	else
	{
		OutAgentState.Reward = -0.01f;
	}

	BuildObservation(OutAgentState.Observations);

	// Reflect logical position in world transform for easier external verification
	FVector Location = GetActorLocation();
	Location.X = PositionX;
	SetActorLocation(Location);
}
