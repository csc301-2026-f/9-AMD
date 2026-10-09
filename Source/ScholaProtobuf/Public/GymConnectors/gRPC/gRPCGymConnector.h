// Copyright (c) 2023 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once

#include "GymConnectors/gRPC/CommunicationManager.h"
#include "GymConnectors/ExternalGymConnector.h"
#include "ProtobufBackends/gRPC/ExchangeRPCBackend.h"
#include "ProtobufBackends/gRPC/ConsumerRPCBackend.h"
#include "ProtobufBackends/gRPC/ProducerRPCBackend.h"
#include "GymConnectors/AutoResetTypeEnum.h"
#include "TrainingSettings/TrainingScriptSettings.h"

THIRD_PARTY_INCLUDES_START
#include "ScholaProtobufMacroGuardBegin.h"
#include "GymConnector.pb.h"
#include "GymConnector.grpc.pb.h"
#include "StateUpdates.pb.h"
#include "State.pb.h"
#include "ScholaProtobufMacroGuardEnd.h"
THIRD_PARTY_INCLUDES_END

#include "gRPCGymConnector.generated.h"

using Schola::InitialState;
using Schola::InitialStateRequest;
using Schola::GymService;
using Schola::GymConnectorStartRequest;
using Schola::GymConnectorStartResponse;
using Schola::TrainingDefinition;
using Schola::TrainingDefinitionRequest;
using Schola::TrainingState;
using Schola::StateUpdate;


/**
 * @brief A connection to an external gym API implemented in Python, using gRPC for communication.
 * @note This can theoretically work with any gRPC client, not just Python, although that is untested currently.
 */
UCLASS(EditInlineNew)
class SCHOLAPROTOBUF_API URPCGymConnector : public UExternalGymConnector
{
	GENERATED_BODY()
private:
	
	/** The service that will handle decision the initial GymConnectorStartRequest. */
	IConsumerBackend<GymConnectorStartRequest>* StartRequestService;
	/** The service that will handle decision requests. */
	IExchangeBackend<Schola::StateUpdate, Schola::State>* DecisionRequestService;
	/** The service that will handle publishing agent definitions. */
	IProducerBackend<Schola::TrainingDefinition>* AgentDefinitionService;

	/** The communication manager that will handle the gRPC server. */
	UPROPERTY()
	TObjectPtr<UCommunicationManager> CommunicationManager;
	// Initialized in Constructor for now

	UPROPERTY()
	EAutoResetType AutoResetType = EAutoResetType::SameStep; // Default to SameStep

	/** Cached training definition for reconnection scenarios */
	FTrainingDefinition CachedAgentDefn;

public:
	
	/** Settings */

	UPROPERTY(EditAnywhere, Category = "Script Settings")
	bool bRunScriptOnPlay = false;

	/** Listen address and port for the gym gRPC service. */
	UPROPERTY(EditAnywhere, Category = "Schola|gRPC")
	FRPCServerSettings ServerSettings;

	/** The settings for the python script to be launched */
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bRunScriptOnPlay", ShowOnlyInnerProperties), Category = "Script Settings")
	FTrainingScriptSettings ScriptSettings;

	/** Runtime handle for the training script process when bRunScriptOnPlay is enabled. */
	UPROPERTY()
	FLaunchableScript Script;

	/** End of Settings */

	/**
	 * @brief Create a new UPythonGymConnector.
	 */
	URPCGymConnector();

	/**
	 * @brief Request a state update from the gym API using the current state of the agents from environments.
	 * @return A future that will eventually contain the decision for all agents in all environments.
	 * @note This function is asynchronous and will return immediately.
	 */
	TFuture<FTrainingStateUpdate*> RequestStateUpdate() override;


	virtual void SubmitState(const FTrainingState& InTrainingState) override;

	virtual void SubmitStateWithInitialState(const FTrainingState& InTrainingState, const FInitialState& InInitialAgentStates) override;

	virtual void SubmitInitialState(const FInitialState& InInitialAgentStates) override;

	/**
	 * @brief Initialize the connector with the given training definitions.
	 * @param[in] AgentDefns The training definitions to initialize with.
	 */
	void Init(const FTrainingDefinition& AgentDefns) override;

	/**
	 * @brief Check if the start request has been received.
	 * @return True if the start request has been received, false otherwise.
	 */
	bool CheckForStart() override;

	EAutoResetType GetAutoResetType() override;
};
