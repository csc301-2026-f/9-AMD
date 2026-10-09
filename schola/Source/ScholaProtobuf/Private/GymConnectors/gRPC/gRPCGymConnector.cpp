// Copyright (c) 2023 Advanced Micro Devices, Inc. All Rights Reserved.

#include "GymConnectors/gRPC/gRPCGymConnector.h"
#include "Misc/Optional.h"
#include "LogScholaProtobuf.h"

URPCGymConnector::URPCGymConnector() {

}

TFuture<FTrainingStateUpdate*> URPCGymConnector::RequestStateUpdate()
{
	return this->DecisionRequestService->Receive<FTrainingStateUpdate>();
}

void URPCGymConnector::SubmitState(const FTrainingState& InTrainingState)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("ScholaProtobuf: gRPCGymConnector Submit State");
	Schola::State StateMsg;
	ProtobufSerializer::ToProto(InTrainingState, StateMsg.mutable_training_state());
	this->DecisionRequestService->Respond(StateMsg);
}

void URPCGymConnector::SubmitStateWithInitialState(const FTrainingState& InTrainingState, const FInitialState& InInitialAgentStates)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("ScholaProtobuf: gRPCGymConnector Submit State With Initial State");
	Schola::State StateMsg;
	ProtobufSerializer::ToProto(InTrainingState, StateMsg.mutable_training_state());
	ProtobufSerializer::ToProto(InInitialAgentStates, StateMsg.mutable_initial_state());
	this->DecisionRequestService->Respond(StateMsg);
}

void URPCGymConnector::SubmitInitialState(const FInitialState& InInitialAgentStates)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("ScholaProtobuf: gRPCGymConnector Submit Initial State");
	Schola::State StateMsg;
	ProtobufSerializer::ToProto(InInitialAgentStates, StateMsg.mutable_initial_state());
	this->DecisionRequestService->Respond(StateMsg);
}

void URPCGymConnector::Init(const FTrainingDefinition& AgentDefns)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("ScholaProtobuf: gRPCGymConnector Init");
	// First-time initialization: create server and services
	std::shared_ptr<GymService::AsyncService> Service = std::make_shared<GymService::AsyncService>();
	this->CommunicationManager = NewObject<UCommunicationManager>();
	int32 Port = 0;
	if (!FParse::Value(FCommandLine::Get(), TEXT("ScholaPort="), Port))
	{
		// Parse failed so we fall back to the default
		Port = this->ServerSettings.Port;
	}
	this->CommunicationManager->Initialize(Port, this->ServerSettings.Address);

	DecisionRequestService = this->CommunicationManager->CreateExchangeBackend<GymService::AsyncService, Schola::StateUpdate, Schola::State>(&GymService::AsyncService::RequestUpdateState, Service);

	// Watch out for clobbering between Schola::AgentDefinition and the parameter

	AgentDefinitionService = this->CommunicationManager->CreateProducerBackend<GymService::AsyncService, TrainingDefinitionRequest, Schola::TrainingDefinition>(&GymService::AsyncService::RequestRequestTrainingDefinition, Service);

	StartRequestService = this->CommunicationManager->CreatePollingBackend<GymService::AsyncService, GymConnectorStartRequest, GymConnectorStartResponse>(&GymService::AsyncService::RequestStartGymConnector, Service);

	// Call initialize
	this->AgentDefinitionService->Initialize();
	this->DecisionRequestService->Initialize();
	this->StartRequestService->Initialize();

	// Prep for the next connection
	this->OnConnectorClosed.AddLambda([this]() {
		UE_LOGFMT(LogScholaProtobuf, Verbose, "URPCGymConnector::Init(): Responding with Empty State Message after connector closed.");
		//Cleanup any carry-over messages, if they still exist.
		this->DecisionRequestService->Reset();
		// Re-publish the training definition for the next connection
		this->AgentDefinitionService->Publish(this->TrainingDefinition);
	});
		
	this->CommunicationManager->StartBackends();

	// Use the config setting, but we can override the config value by passing ScholaDisableScript on the command line
	if (this->bRunScriptOnPlay && !FParse::Param(FCommandLine::Get(), TEXT("ScholaDisableScript")))
	{
		FScriptArgBuilder ArgBuilder = FScriptArgBuilder();
		this->ScriptSettings.GetArgs(ArgBuilder);
		this->ServerSettings.GetArgs(ArgBuilder);
		FString Args = ArgBuilder.Build();

		this->Script = this->ScriptSettings.GetLaunchableScript();
		Script.AppendArgs(Args);
		Script.LaunchScript();
	}
	this->AgentDefinitionService->Publish(AgentDefns);
}

bool URPCGymConnector::CheckForStart()
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("ScholaProtobuf: gRPCGymConnector CheckForStart");
	TOptional<FStartRequest> OptionalStartRequest = TOptional<FStartRequest>();
	this->StartRequestService->Poll(OptionalStartRequest);
	if (OptionalStartRequest.IsSet())
	{
		FStartRequest& StartRequest = OptionalStartRequest.GetValue();
		this->AutoResetType = StartRequest.AutoResetType;
		
		this->SetStatus(EConnectorStatus::Running);
	}
	return this->Status == EConnectorStatus::Running;
}

EAutoResetType URPCGymConnector::GetAutoResetType()
{
	//Log that we got into the function in PythonGymConnector
	return this->AutoResetType;
}
