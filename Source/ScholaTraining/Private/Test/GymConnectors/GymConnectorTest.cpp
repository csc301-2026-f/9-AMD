// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"

#include "CoreGlobals.h"
#include "Engine/World.h"
#include "GymConnectors/ManualGymConnector.h"
#include "GymConnectors/AbstractGymConnector.h"
#include "Points/DiscretePoint.h"
#include "Points/BoxPoint.h"
#include "StepEnv.h"

// Helpers for converting TrainingState to arrays for easier verification

void GetCountEnvTerminatedArray(const FTrainingState& InState, TArray<bool>& OutArray)
{
	OutArray.Empty();
	for (const FEnvironmentState& EnvState : InState.EnvironmentStates)
	{
		OutArray.Add(EnvState.AgentStates["StepAgent"].bTerminated);
	}
}

void GetCountEnvTruncatedArray(const FTrainingState& InState, TArray<bool>& OutArray)
{
	OutArray.Empty();
	for (const FEnvironmentState& EnvState : InState.EnvironmentStates)
	{
		OutArray.Add(EnvState.AgentStates["StepAgent"].bTruncated);
	}
}

void GetCountEnvObsArray(const FTrainingState& InState, TArray<int>& OutArray)
{
	OutArray.Empty();
	for (const FEnvironmentState& EnvState : InState.EnvironmentStates)
	{
		OutArray.Add(EnvState.AgentStates["StepAgent"].Observations.Get<FBoxPoint>().Values[0]);
	}
}

void GetCountEnvObsArray(const FInitialState& InState, TArray<int>& OutArray)
{
	for (const auto& EnvStatePair : InState.EnvironmentStates)
	{
		OutArray[EnvStatePair.Key] = EnvStatePair.Value.AgentStates["StepAgent"].Observations.Get<FBoxPoint>().Values[0];
	}
}

// Helpers for working with Arrays in Tests
//TODO clean up this naming scheme Inner vs 
template<typename T>
FString InternalArrToStr(const TArray<T>& InArray, FString(*ElemToStrFunc)(T))
{
	FString OutputStr = FString("{ ");
	bool	bFirstElem = true;
	for (T Elem : InArray)
	{
		if (bFirstElem)
		{
			bFirstElem = false;
		}
		else
		{
			OutputStr += TEXT(", ");
		}
		OutputStr += ElemToStrFunc(Elem);
	}
	OutputStr += TEXT("}");
	return OutputStr;
}

FString ArrToStr(const TArray<bool>& InArray)
{
	return InternalArrToStr<bool>(InArray, [](bool Elem) { return Elem ? FString("true") : FString("false"); });
}

FString ArrToStr(const TArray<int>& InArray)
{
	return InternalArrToStr<int>(InArray, &FString::FromInt);
}

FString ArrToStr(const TArray<float>& InArray)
{
	return InternalArrToStr<float>(InArray, [](float Elem) { return FString::SanitizeFloat(Elem); });
}


template <typename T>
bool InternalTestArrayEqual(FAutomationTestBase* Test, const TCHAR* What, const TArray<T>& Actual, const TArray<T>& Expected)
{
	if (Actual != Expected)
	{
		Test->AddError(FString::Printf(TEXT("Expected '%s' to be %s, but it was %s."), What, *ArrToStr(Expected), *ArrToStr(Actual)), 1);
		return false;
	}
	return true;
}

#define TestArrayEqual(What, Actual, Expected) InternalTestArrayEqual(this, What, Actual, Expected)






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


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSameStepAutoResetGymConnectorTest, "Schola.GymConnectors.AutoReset SameStep", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FSameStepAutoResetGymConnectorTest::RunTest(const FString& Parameters)
{
	int N_ENVS = 4;
	TArray<TScriptInterface<IBaseScholaEnvironment>> Envs;
	
	for(int i = 0; i < N_ENVS; i++)
	{
		UStepEnv* TempEnv = NewObject<UStepEnv>();
		TempEnv->MaxSteps = i + 5;
		Envs.Add(TempEnv);
	}// 5, 6, 7, 8 max steps for each environment

	//Make a Gym Connector
	UManualGymConnector* GymConnector = NewObject<UManualGymConnector>();
	GymConnector->AutoResetType = EAutoResetType::SameStep;
	GymConnector->UAbstractGymConnector::Init(Envs);

	TArray<TMap<FString, TInstancedStruct<FPoint>>> ZeroActions;
	ZeroActions.AddDefaulted(N_ENVS);
	for (int i = 0; i < N_ENVS; i++)
	{
		TMap<FString, TInstancedStruct<FPoint>>& ActionMap = ZeroActions[i];
		ActionMap.Emplace("StepAgent", TInstancedStruct<FPoint>::Make<FDiscretePoint>(0));
	}
	
	FInitialState InitState;
	FTrainingState TrainState;
	GymConnector->ManualReset({},{}, InitState);
	// Start at 1 because the Step function increments before checking for done. So p
	for (int Step = 1; Step < N_ENVS + 5; Step++) // 1 ... 8
	{
		GymConnector->ManualStep(ZeroActions, InitState, TrainState);
		TestEqual(TEXT("TrainState.EnvironmentStates.Num() == N_ENVS"), TrainState.EnvironmentStates.Num(), N_ENVS);

		for (int EnvId = 0; EnvId < N_ENVS; EnvId++) //0, 1, 2, 3
		{
			if (!TrainState.EnvironmentStates[EnvId].AgentStates["StepAgent"].bTerminated)
			{
				TestNotEqual(TEXT("Done is False, but the Step is == to the Target"), Step, EnvId + 5);
				TestFalse(TEXT("Initial Observations supplied when no environments are Done."), InitState.EnvironmentStates.Contains(EnvId));
			}
			else
			{
				TestEqual(TEXT("Done is True, but the Step is != to the Target"), Step, EnvId + 5);
				TestTrue(TEXT("Initial Observations Missing."), InitState.EnvironmentStates.Contains(EnvId));
				
				const float InitialObs = InitState.EnvironmentStates[EnvId].AgentStates["StepAgent"].Observations.GetMutable<FBoxPoint>().Values[0];
				const float TerminalObs = TrainState.EnvironmentStates[EnvId].AgentStates["StepAgent"].Observations.GetMutable<FBoxPoint>().Values[0];
				
				TestLessThan(TEXT("Initial Observations should be less than Terminal Observations"), InitialObs, TerminalObs);
				TestEqual(TEXT("Initial Obs != 0"), (int)InitialObs, 0);
				TestEqual(TEXT("Terminal Obs != MaxSteps"), (int)TerminalObs, EnvId + 5);
			}
		}
	}
	return true;
}

// end of adapted code

// The below code is adapted from https://github.com/DLR-RM/stable-baselines3/blob/master/tests/test_vec_envs.py

/* The MIT License

Copyright (c) 2016 OpenAI
Copyright (c) 2022 Farama Foundation

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


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNextStepAutoResetGymConnectorTest, "Schola.GymConnectors.AutoReset NextStep", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FNextStepAutoResetGymConnectorTest::RunTest(const FString& Parameters)
{
	int N_ENVS = 3;
	TArray<int>					MaxSteps = { 2, 3, 3};
	TArray<TScriptInterface<IBaseScholaEnvironment>> Envs;
	
	for(int i = 0; i < N_ENVS; i++)
	{
		UStepEnv* TempEnv = NewObject<UStepEnv>();
		TempEnv->MaxSteps = MaxSteps[i];
		Envs.Add(TempEnv);
	} // 2, 3, 3 max steps for each environment

	//Make a Gym Connector
	UManualGymConnector* GymConnector = NewObject<UManualGymConnector>();
	GymConnector->AutoResetType = EAutoResetType::NextStep;
	GymConnector->UAbstractGymConnector::Init(Envs);

	TArray<TMap<FString, TInstancedStruct<FPoint>>> ZeroActions;
	ZeroActions.AddDefaulted(N_ENVS);
	for (int i = 0; i < N_ENVS; i++)
	{
		TMap<FString, TInstancedStruct<FPoint>>& ActionMap = ZeroActions[i];
		ActionMap.Emplace("StepAgent", TInstancedStruct<FPoint>::Make<FDiscretePoint>(0));
	}

	TArray<int>	 ObsArray = {-1, -1, -1};
	TArray<bool> TerminatedArray = { false, false, false };
	TArray<bool> TruncatedArray = { false, false, false };

	FInitialState InitState;
	FTrainingState TrainState;
	
	// Verify Setup

	TestEqual(TEXT("AutoResetType"), GymConnector->GetAutoResetType(), EAutoResetType::NextStep);

	// Initial Reset
	
	GymConnector->ManualReset({},{}, InitState);
	GetCountEnvObsArray(InitState, ObsArray);

	TestArrayEqual(TEXT("Initial State"), ObsArray, TArray<int>({ 0, 0, 0 }));

	// Step 1

	GymConnector->ManualStep(ZeroActions, InitState, TrainState);
	GetCountEnvObsArray(TrainState, ObsArray);
	GetCountEnvTerminatedArray(TrainState, TerminatedArray);
	GetCountEnvTruncatedArray(TrainState, TruncatedArray);

	TestArrayEqual(TEXT("Step 1 Obs"), ObsArray, TArray<int>({ 1, 1, 1}));
	TestArrayEqual(TEXT("Step 1 Terminated"), TerminatedArray, TArray<bool>({ false, false, false }));
	TestArrayEqual(TEXT("Step 1 Truncated"), TruncatedArray, TArray<bool>({ false, false, false }));

	// Step 2

	GymConnector->ManualStep(ZeroActions, InitState, TrainState);
	GetCountEnvObsArray(TrainState, ObsArray);
	GetCountEnvTerminatedArray(TrainState, TerminatedArray);
	GetCountEnvTruncatedArray(TrainState, TruncatedArray);

	TestArrayEqual(TEXT("Step 2 Obs"), ObsArray, TArray<int>({ 2, 2, 2 }));
	TestArrayEqual(TEXT("Step 2 Terminated"), TerminatedArray, TArray<bool>({ true, false, false }));
	TestArrayEqual(TEXT("Step 2 Truncated"), TruncatedArray, TArray<bool>({ false, false, false }));

	// Step 3

	GymConnector->ManualStep(ZeroActions, InitState, TrainState);
	GetCountEnvObsArray(TrainState, ObsArray);
	GetCountEnvTerminatedArray(TrainState, TerminatedArray);
	GetCountEnvTruncatedArray(TrainState, TruncatedArray);

	TestArrayEqual(TEXT("Step 3 Obs"), ObsArray, TArray<int>({ 0, 3, 3 }));
	TestArrayEqual(TEXT("Step 3 Terminated"), TerminatedArray, TArray<bool>({ false, true, true }));
	TestArrayEqual(TEXT("Step 3 Truncated"), TruncatedArray, TArray<bool>({ false, false, false }));
	
	// Step 4

	GymConnector->ManualStep(ZeroActions, InitState, TrainState);
	GetCountEnvObsArray(TrainState, ObsArray);
	GetCountEnvTerminatedArray(TrainState, TerminatedArray);
	GetCountEnvTruncatedArray(TrainState, TruncatedArray);

	TestArrayEqual(TEXT("Step 4 Obs"), ObsArray, TArray<int>({ 1, 0, 0 }));
	TestArrayEqual(TEXT("Step 4 Terminated"), TerminatedArray, TArray<bool>({ false, false, false }));
	TestArrayEqual(TEXT("Step 4 Truncated"), TruncatedArray, TArray<bool>({ false, false, false }));
	return true;
}

// end of adapted code
