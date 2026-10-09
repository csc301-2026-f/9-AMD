// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "ProtobufUtils/ProtobufSerializer.h"
#include "Spaces/DictSpace.h"
#include <ImitationDataTypes/ImitationTrainingState.h>
#include "LogScholaProtobuf.h"

template<>
void ProtobufSerializer::ToProto(const FTrainingState& InTrainingState, Schola::TrainingState* OutTrainingStateProto)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("ScholaProtobuf: Serialize TrainingState");
	ProtobufSerializer::ToProto(InTrainingState.EnvironmentStates, OutTrainingStateProto->mutable_environment_states());
}

template<>
void ProtobufSerializer::ToProto(const FEnvironmentState& InEnvironmentState, Schola::EnvironmentState* OutEnvironmentStateProto)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("ScholaProtobuf: Serialize EnvironmentState");
	ProtobufSerializer::ToProto(InEnvironmentState.AgentStates, OutEnvironmentStateProto->mutable_agent_states());
}

template <>
void ProtobufSerializer::ToProto(const FAgentState& InAgentState, Schola::AgentState* OutAgentStateProto)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("ScholaProtobuf: Serialize AgentState");
	ToProto(InAgentState.Info, OutAgentStateProto->mutable_info());
	ToProto(InAgentState.Observations, OutAgentStateProto->mutable_observations());
	
	OutAgentStateProto->set_reward(InAgentState.Reward);
	OutAgentStateProto->set_terminated(InAgentState.bTerminated);
	OutAgentStateProto->set_truncated(InAgentState.bTruncated);
	
}

template <>
void ProtobufSerializer::ToProto(const FInitialState& InInitialState, Schola::InitialState* OutInitialStateProto)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("ScholaProtobuf: Serialize InitialState");
	ToProto(InInitialState.EnvironmentStates, OutInitialStateProto->mutable_environment_states());
}

template <>
void ProtobufSerializer::ToProto(const FInitialEnvironmentState& InInitialEnvironmentState, Schola::InitialEnvironmentState* OutInitialEnvironmentStateProto)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("ScholaProtobuf: Serialize InitialEnvironmentState");
	ToProto(InInitialEnvironmentState.AgentStates, OutInitialEnvironmentStateProto->mutable_agent_states());
}

template <>
void ProtobufSerializer::ToProto(const FInitialAgentState& InInitialAgentState, Schola::InitialAgentState* OutInitialAgentStateProto)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("ScholaProtobuf: Serialize InitialAgentState");
	ToProto(InInitialAgentState.Info, OutInitialAgentStateProto->mutable_info());
	ToProto(InInitialAgentState.Observations, OutInitialAgentStateProto->mutable_observations());
}

template <>
void ProtobufSerializer::ToProto(const FTrainingDefinition& InTrainingDefinition, Schola::TrainingDefinition* OutTrainingDefinitionProto)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("ScholaProtobuf: Serialize TrainingDefinition");
	ToProto(InTrainingDefinition.EnvironmentDefinitions, OutTrainingDefinitionProto->mutable_environment_definitions());
}

template <>
void ProtobufSerializer::ToProto(const FInteractionDefinition& InInteractionDefinition, Schola::AgentDefinition* OutInteractionDefinitionProto)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("ScholaProtobuf: Serialize InteractionDefinition");
	OutInteractionDefinitionProto->set_agent_type(TCHAR_TO_UTF8(*InInteractionDefinition.AgentType));

	if (!InInteractionDefinition.ObsSpaceDefn.IsValid())
	{
		UE_LOGFMT(LogScholaProtobuf, Error, "ProtobufSerializer::ToProto<FInteractionDefinition>(): ObsSpaceDefn is not valid!");
	}
	
	ToProto(InInteractionDefinition.ObsSpaceDefn, OutInteractionDefinitionProto->mutable_obs_space());
	
	if (!InInteractionDefinition.ActionSpaceDefn.IsValid())
	{
		UE_LOGFMT(LogScholaProtobuf, Error, "ProtobufSerializer::ToProto<FInteractionDefinition>(): ActionSpaceDefn is not valid! The action space TInstancedStruct was not initialized");
	}
	
	ToProto(InInteractionDefinition.ActionSpaceDefn, OutInteractionDefinitionProto->mutable_action_space());
}

template <>
void ProtobufSerializer::ToProto(const FEnvironmentDefinition& InEnvironmentDefinition, Schola::EnvironmentDefinition* OutEnvironmentDefinition)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("ScholaProtobuf: Serialize EnvironmentDefinition");
	ToProto(InEnvironmentDefinition.AgentDefinitions, OutEnvironmentDefinition->mutable_agent_definitions());
};


template <>
void ProtobufSerializer::ToProto(const FImitationTrainingState& InState, Schola::ImitationTrainingState* OutStateProto)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("ScholaProtobuf: Serialize ImitationTrainingState");
	ToProto(InState.EnvironmentStates, OutStateProto->mutable_environment_states());
}

template <>
void ProtobufSerializer::ToProto(const FImitationEnvironmentState& InEnvState, Schola::ImitationEnvironmentState* OutEnvStateProto)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("ScholaProtobuf: Serialize ImitationEnvironmentState");
	ToProto(InEnvState.AgentStates, OutEnvStateProto->mutable_agent_states());
}

template <>
void ProtobufSerializer::ToProto(const FImitationAgentState& InAgentState, Schola::ImitationAgentState* OutAgentStateProto)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("ScholaProtobuf: Serialize ImitationAgentState");
	ToProto(InAgentState.Info, OutAgentStateProto->mutable_info());
	ToProto(InAgentState.Observations, OutAgentStateProto->mutable_observations());
	ToProto(InAgentState.Actions, OutAgentStateProto->mutable_actions());

	OutAgentStateProto->set_reward(InAgentState.Reward);
	OutAgentStateProto->set_terminated(InAgentState.bTerminated);
	OutAgentStateProto->set_truncated(InAgentState.bTruncated);
}




template <>
void ProtobufSerializer::ToProto(const TInstancedStruct<FSpace>& InSpace, Schola::Space* OutSpaceProto)
{
	if (!InSpace.IsValid())
	{
		UE_LOGFMT(LogScholaProtobuf, Error, "ProtobufSerializer::ToProto<TInstancedStruct<FSpace>>(): InSpace is not valid/initialized! This means an actuator or sensor returned an empty action/observation space.");
		check(false); // This will give us a better error message
		return;
	}
	
	// If it's a DictSpace, check each individual space within it
	if (InSpace.GetScriptStruct() == FDictSpace::StaticStruct())
	{
		const FDictSpace& DictSpace = InSpace.Get<FDictSpace>();
		for (const TPair<FString, TInstancedStruct<FSpace>>& Pair : DictSpace.Spaces)
		{
			if (!Pair.Value.IsValid())
			{
				UE_LOGFMT(LogScholaProtobuf, Error, "ProtobufSerializer::ToProto<TInstancedStruct<FSpace>>(): DictSpace contains invalid space for key '{0}'. This sensor or actuator did not properly initialize its action/observation space.", Pair.Key);
				check(false);
				return;
			}
		}
	}
	
	ProtobufSpaceSerializer SpaceSerializer = { OutSpaceProto };
	InSpace.Get<FSpace>().Accept(SpaceSerializer);
};

template<>
void ProtobufSerializer::ToProto(const TInstancedStruct<FPoint>& InPoint, Schola::Point* OutPointProto)
{
	if (!InPoint.IsValid())
	{
		UE_LOGFMT(LogScholaProtobuf, Error, "ProtobufSerializer::ToProto<TInstancedStruct<FPoint>>(): InPoint is not valid/initialized! An observation/action point was expected but the TInstancedStruct is empty. Ensure CollectObservations/Act is initializing the struct with InitializeAs<...>().");
		// Leave proto empty (oneof unset) so downstream will raise a clear ValueError in Python.
		return;
	}
	ProtobufPointSerializer PointSerializer = { OutPointProto };
	InPoint.Get<FPoint>().Accept(PointSerializer);
};


template<>
void ProtobufSerializer::ToProto(const FString& InUnrealString, std::string* OutProtobufString)
{
	*OutProtobufString = TCHAR_TO_UTF8(*InUnrealString);
}

