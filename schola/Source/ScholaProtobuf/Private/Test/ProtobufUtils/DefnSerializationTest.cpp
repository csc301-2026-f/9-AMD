// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"

#include "ProtobufUtils/ProtobufSerializer.h"

#include "TrainingDataTypes/EnvironmentDefinition.h"
#include "TrainingDataTypes/TrainingDefinition.h"

#include "Common/InteractionDefinition.h"
#include "Spaces/DiscreteSpace.h"

#include <string>

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FProtobufEnvironmentDefinitionSerializationTest, "Schola.Protobuf.Serialization.Definitions.EnvironmentDefinition", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FProtobufEnvironmentDefinitionSerializationTest::RunTest(const FString& Parameters)
{
	// Build InteractionDefinition with discrete spaces for obs and action
	FInteractionDefinition Inter;
	Inter.AgentType = TEXT("Tagger");
	Inter.ObsSpaceDefn.InitializeAs<FDiscreteSpace>();
	FDiscreteSpace* ObsSpace = Inter.ObsSpaceDefn.GetMutablePtr<FDiscreteSpace>();
	ObsSpace->High = 5;

	Inter.ActionSpaceDefn.InitializeAs<FDiscreteSpace>();
	FDiscreteSpace* ActSpace = Inter.ActionSpaceDefn.GetMutablePtr<FDiscreteSpace>();
	ActSpace->High = 3;

	FEnvironmentDefinition EnvDef;
	EnvDef.AgentDefinitions.Add(TEXT("agentA"), Inter);

	Schola::EnvironmentDefinition OutProto;
	ProtobufSerializer::ToProto(EnvDef, &OutProto);

	// Proto agent_definitions is a std::map<std::string, Schola::AgentDefinition>
	const auto& Map = OutProto.agent_definitions();
	auto It = Map.find(std::string(TCHAR_TO_UTF8(TEXT("agentA"))));
	TestTrue(TEXT("EnvironmentDefinition contains 'agentA'"), It != Map.end());
	if (It != Map.end())
	{
		const Schola::AgentDefinition& AgentProto = It->second;
		TestEqual(TEXT("AgentProto.agent_type == Tagger"), FString(UTF8_TO_TCHAR(AgentProto.agent_type().c_str())), FString(TEXT("Tagger")));
		TestTrue(TEXT("AgentProto has obs_space serialized"), AgentProto.has_obs_space());
		if (AgentProto.has_obs_space())
		{
			TestTrue(TEXT("ObsSpace is discrete_space"), AgentProto.obs_space().has_discrete_space());
			if (AgentProto.obs_space().has_discrete_space())
			{
				TestEqual(TEXT("ObsSpace.high == 5"), AgentProto.obs_space().discrete_space().high(), 5);
			}
		}

		TestTrue(TEXT("AgentProto has action_space serialized"), AgentProto.has_action_space());
		if (AgentProto.has_action_space())
		{
			TestTrue(TEXT("ActionSpace is discrete_space"), AgentProto.action_space().has_discrete_space());
			if (AgentProto.action_space().has_discrete_space())
			{
				TestEqual(TEXT("ActionSpace.high == 3"), AgentProto.action_space().discrete_space().high(), 3);
			}
		}
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FProtobufTrainingDefinitionSerializationTest, "Schola.Protobuf.Serialization.Definitions.TrainingDefinition", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FProtobufTrainingDefinitionSerializationTest::RunTest(const FString& Parameters)
{
	// Reuse environment definition from previous test
	FInteractionDefinition Inter;
	Inter.AgentType = TEXT("Runner");
	Inter.ObsSpaceDefn.InitializeAs<FDiscreteSpace>();
	Inter.ObsSpaceDefn.GetMutablePtr<FDiscreteSpace>()->High = 2;
	Inter.ActionSpaceDefn.InitializeAs<FDiscreteSpace>();
	Inter.ActionSpaceDefn.GetMutablePtr<FDiscreteSpace>()->High = 4;

	FEnvironmentDefinition EnvDef;
	EnvDef.AgentDefinitions.Add(TEXT("agentA"), Inter);

	FTrainingDefinition TrainDef;
	TrainDef.EnvironmentDefinitions.Add(EnvDef);

	Schola::TrainingDefinition OutProto;
	ProtobufSerializer::ToProto(TrainDef, &OutProto);

	// Check environment_definitions repeated field
	TestTrue(TEXT("TrainingDefinition has 1 environment_definition"), OutProto.environment_definitions_size() == 1);
	if (OutProto.environment_definitions_size() == 1)
	{
		const Schola::EnvironmentDefinition& EnvProto = OutProto.environment_definitions(0);
		const auto& Map = EnvProto.agent_definitions();
		auto It = Map.find(std::string(TCHAR_TO_UTF8(TEXT("agentA"))));
		TestTrue(TEXT("EnvironmentDefinition[0] contains 'agentA'"), It != Map.end());
		if (It != Map.end())
		{
			const Schola::AgentDefinition& AgentProto = It->second;
			TestEqual(TEXT("AgentProto.agent_type == Runner"), FString(UTF8_TO_TCHAR(AgentProto.agent_type().c_str())), FString(TEXT("Runner")));
			TestTrue(TEXT("AgentProto.obs_space present"), AgentProto.has_obs_space());
			if (AgentProto.has_obs_space())
			{
				TestTrue(TEXT("ObsSpace is discrete_space"), AgentProto.obs_space().has_discrete_space());
				if (AgentProto.obs_space().has_discrete_space())
				{
					TestEqual(TEXT("ObsSpace.high == 2"), AgentProto.obs_space().discrete_space().high(), 2);
				}
			}
		}
	}

	return true;
}
