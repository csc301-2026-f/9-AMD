// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"

#include "ProtobufUtils/ProtobufSerializer.h"

#include "TrainingDataTypes/EnvironmentState.h"
#include "TrainingDataTypes/TrainingState.h"

#include "Points/Point.h"
#include "Points/DiscretePoint.h"

#include <string>

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FProtobufAgentStateSerializationTest, "Schola.Protobuf.Serialization.Training.AgentState", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FProtobufAgentStateSerializationTest::RunTest(const FString& Parameters)
{
	// Observations: simple discrete point
	TInstancedStruct<FPoint> Obs;
	Obs.InitializeAs<FDiscretePoint>();
	FDiscretePoint* D = Obs.GetMutablePtr<FDiscretePoint>();
	D->Value = 7;

	// Info map
	TMap<FString, FString> Info;
	Info.Add(TEXT("team"), TEXT("red"));

	// Agent state: reward and status Completed
	FAgentState Agent(Obs, 3.5f, true, false, Info);

	Schola::AgentState OutProto;
	ProtobufSerializer::ToProto(Agent, &OutProto);

	// Reward
	TestTrue(TEXT("Agent reward is set"), FMath::IsNearlyEqual(OutProto.reward(), 3.5f));
	// Status mapping
	TestTrue(TEXT("Agent is marked Completed"), OutProto.terminated());
	// Info map
	{
		const auto& Map = OutProto.info();
		auto It = Map.find(std::string(TCHAR_TO_UTF8(TEXT("team"))));
		TestTrue(TEXT("Info contains 'team'"), It != Map.end());
		if (It != Map.end())
		{
			TestTrue(TEXT("Info value == 'red'"), It->second == std::string(TCHAR_TO_UTF8(TEXT("red"))));
		}
	}
	// Observations serialized
	TestTrue(TEXT("Observations serialized on agent"), OutProto.has_observations());
	if (OutProto.has_observations())
	{
		TestTrue(TEXT("Observation is discrete_point"), OutProto.observations().has_discrete_point());
		if (OutProto.observations().has_discrete_point())
		{
			TestEqual(TEXT("Observation value == 7"), OutProto.observations().discrete_point().value(), 7);
		}
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FProtobufEnvironmentStateSerializationTest, "Schola.Protobuf.Serialization.Training.EnvironmentState", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FProtobufEnvironmentStateSerializationTest::RunTest(const FString& Parameters)
{
	// Create a simple agent and environment
	TInstancedStruct<FPoint> Obs;
	Obs.InitializeAs<FDiscretePoint>();
	Obs.GetMutablePtr<FDiscretePoint>()->Value = 9;

	TMap<FString, FString> Info;
	Info.Add(TEXT("role"), TEXT("worker"));

	FAgentState Agent(Obs, 1.25f, false, false, Info);

	FEnvironmentState Env;
	Env.AgentStates.Add(TEXT("agentA"), Agent);

	Schola::EnvironmentState OutProto;
	ProtobufSerializer::ToProto(Env, &OutProto);

	// AgentStates map should contain "agentA"
	const auto& Map = OutProto.agent_states();
	auto It = Map.find(std::string(TCHAR_TO_UTF8(TEXT("agentA"))));
	TestTrue(TEXT("Environment contains agentA"), It != Map.end());
	if (It != Map.end())
	{
		const Schola::AgentState& AgentProto = It->second;
		TestTrue(TEXT("AgentProto has reward field set"), FMath::IsNearlyEqual(AgentProto.reward(), 1.25f));
		TestFalse(TEXT("AgentProto is not completed or truncated"), AgentProto.terminated() || AgentProto.truncated());
		// Observations check
		TestTrue(TEXT("AgentProto has observations"), AgentProto.has_observations());
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FProtobufInitialStateSerializationTest, "Schola.Protobuf.Serialization.Training.InitialState.TMapInt", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FProtobufInitialStateSerializationTest::RunTest(const FString& Parameters)
{
	// Create an initial environment with one initial agent state
	TInstancedStruct<FPoint> Obs = TInstancedStruct<FPoint>::Make<FDiscretePoint>(2);

	TMap<FString, FInitialAgentState> AgentMap;
	FInitialAgentState InitAgent(Obs, TMap<FString, FString>({{TEXT("k"), TEXT("v")}}));
	AgentMap.Add(TEXT("a1"), InitAgent);

	FInitialEnvironmentState InitEnv;
	InitEnv.AgentStates = AgentMap;

	// Create FInitialState with TMap<int, FInitialEnvironmentState> keyed by 0 and 2
	FInitialState InitialState;
	InitialState.EnvironmentStates.Add(0, InitEnv);
	InitialState.EnvironmentStates.Add(2, InitEnv);

	Schola::InitialState OutProto;
	ProtobufSerializer::ToProto(InitialState, &OutProto);

	// protobuf map should contain keys 0 and 2
	const auto& Map = OutProto.environment_states();
	TestTrue(TEXT("Contains key 0"), Map.find(0) != Map.end());
	TestTrue(TEXT("Contains key 2"), Map.find(2) != Map.end());
	TestTrue(TEXT("Does not contain key 1"), Map.find(1) == Map.end());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FProtobufTrainingStateSerializationTest, "Schola.Protobuf.Serialization.Training.TrainingState.Multi Env Test", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FProtobufTrainingStateSerializationTest::RunTest(const FString& Parameters)
{
	// Build two environment states (empty)
	FEnvironmentState Env0;
	FEnvironmentState Env1;
	FAgentState		  DefaultAgentState = FAgentState(TInstancedStruct<FPoint>::Make<FDiscretePoint>(2), 0.00, false,false, {});
	
	Env0.AgentStates.Add(TEXT("a"), DefaultAgentState);
	Env1.AgentStates.Add(TEXT("b"), DefaultAgentState);

	FTrainingState TrainingState;
	TrainingState.EnvironmentStates.Add(Env0);
	TrainingState.EnvironmentStates.Add(Env1);

	Schola::TrainingState OutProto;
	ProtobufSerializer::ToProto(TrainingState, &OutProto);

	const auto& Map = OutProto.environment_states();
	// Should have keys 0 and 1 populated from the array-to-map template
	TestTrue(TEXT("TrainingState environment_states size == 2"), (int)Map.size() == 2);
	TestTrue(TEXT("Has key 0"), Map[0].agent_states().contains("a"));
	TestTrue(TEXT("Has key 1"), Map[1].agent_states().contains("b"));

	return true;
}
