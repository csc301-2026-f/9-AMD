// The below code is adapted from https://github.com/DLR-RM/stable-baselines3/blob/master/tests/test_vec_envs.py

/* 
The MIT License

Copyright (c) 2019 Antonin Raffin

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

HE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

// Modifications Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "StepEnv.h"

#include "Spaces/BoxSpace.h"
#include "Spaces/DiscreteSpace.h"
#include "Points/BoxPoint.h"
#include "Points/DiscretePoint.h"
#include "Common/LogSchola.h"

UStepEnv::UStepEnv(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    
}

void UStepEnv::InitializeEnvironment_Implementation(TMap<FString, FInteractionDefinition>& OutAgentDefinitions)
{
    FInteractionDefinition Defn;

    // Observation: single Box space with range [0,999] (matches gym.spaces.Box)
    TInstancedStruct<FSpace>& ObsSpace = Defn.ObsSpaceDefn;
	ObsSpace.InitializeAs<FBoxSpace>();
    FBoxSpace& Box = ObsSpace.GetMutable<FBoxSpace>();
    Box.Add(0.0f, 999.0f);

    // Action: a single discrete space with 2 choices (matches gym.spaces.Discrete(2))
    TInstancedStruct<FSpace>& ActSpace = Defn.ActionSpaceDefn;
    ActSpace.InitializeAs<FDiscreteSpace>(2);

    OutAgentDefinitions.Add(AgentName, Defn);
}

void UStepEnv::Reset_Implementation(TMap<FString, FInitialAgentState>& OutAgentState)
{
    // Reset to step 0
    CurrentStep = 0;
    FInitialAgentState& AgentState = OutAgentState.FindOrAdd(AgentName);
    BuildObservation(AgentState.Observations);
}

void UStepEnv::BuildObservation(TInstancedStruct<FPoint>& OutObservation) const
{
    OutObservation.InitializeAs<FBoxPoint>();
    FBoxPoint& Box = OutObservation.GetMutable<FBoxPoint>();
    Box.Values = { static_cast<float>(CurrentStep) };
}

void UStepEnv::Step_Implementation(const TMap<FString, FInstancedStruct>& InActions, TMap<FString, FAgentState>& OutAgentStates)
{
    // Mirror Python behavior: return prev_step as observation, reward 0.0, terminated=false,
    // truncated when current_step >= MaxSteps

    CurrentStep += 1;

    FAgentState& AgentState = OutAgentStates.Add(AgentName);
    
    AgentState.Reward = 0.0f;
    AgentState.bTerminated = (MaxSteps > 0) && (CurrentStep >= MaxSteps);

  
    // Build observation that represents CurrentStep
	BuildObservation(AgentState.Observations);
}
