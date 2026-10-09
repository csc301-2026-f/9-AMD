// Copyright (c) 2023-2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "GymConnectors/gRPC/CommunicationManager.h"
#include "LogScholaProtobuf.h"

bool UCommunicationManager::RegisterService(std::shared_ptr<grpc::Service> Service)
{
	// Add this service if it doesn't already exist
	// We only track this for registration purposes so don't take ownership

	if (Service == nullptr)
	{
		UE_LOGFMT(LogScholaProtobuf, Warning, "UCommunicationManager::RegisterService(): Service is null - Skipping");
		return false;
	}
	else if (RegisteredServices.count(Service.get()) == 0)
	{
		UE_LOGFMT(LogScholaProtobuf, Verbose, "UCommunicationManager::RegisterService(): Service registered");
		Builder->RegisterService(Service.get());
		RegisteredServices.emplace(Service.get());
		return true;
	}
	else
	{
		UE_LOGFMT(LogScholaProtobuf, Verbose, "UCommunicationManager::RegisterService(): Service exists - Skipping registration");
		return false;
	}
}

std::unique_ptr<ServerCompletionQueue> UCommunicationManager::GetCompletionQueue()
{
	return Builder->AddCompletionQueue();
}

void UCommunicationManager::ShutdownServer()
{
	this->State = EComSystemState::NOTSTARTED;
	UE_LOGFMT(LogScholaProtobuf, Verbose, "UCommunicationManager::ShutdownServer(): Cleaning up server");
	// Stop receiving RPC requests
	if (Server != nullptr)
	{
		// Shut the server down with a right now deadline
		Server->Shutdown(gpr_inf_past(GPR_CLOCK_MONOTONIC));
	}
	else
	{
		UE_LOGFMT(LogScholaProtobuf, Warning, "UCommunicationManager::ShutdownServer(): Server was null");
	}
	UE_LOGFMT(LogScholaProtobuf, Verbose, "UCommunicationManager::ShutdownServer(): Server shutdown - Closing CQueues");
	// broadcast to all the backends to clean up their resources
	OnServerShutdownDelegate.Broadcast();
	UE_LOGFMT(LogScholaProtobuf, Verbose, "UCommunicationManager::ShutdownServer(): All CQueues closed");
}

UCommunicationManager::~UCommunicationManager()
{
	if (this->State == EComSystemState::STARTED)
	{
		// Keep these seperate incase we want to make this restartable
		this->ShutdownServer();
	}
	delete this->Builder;
	this->Builder = nullptr;
}

bool UCommunicationManager::StartBackends()
{

	Server = Builder->BuildAndStart();

	if (Server == nullptr)
	{
		UE_LOGFMT(LogScholaProtobuf, Error, "UCommunicationManager::StartBackends(): Server not started - Address {0} unavailable or not all services were started", ServerURL);
		this->State = EComSystemState::FAILURE;
		return false;
	}
	else
	{
		UE_LOGFMT(LogScholaProtobuf, Log, "UCommunicationManager::StartBackends(): Running server on {0}", ServerURL);

		// Perform initialization of the server
		this->OnServerStartDelegate.Broadcast();
		this->State = EComSystemState::STARTED;

		// Call Each Communication Interfaces Establish methods
		this->OnServerReadyDelegate.Broadcast();

		// Send any initial messages (e.g. Space Definitions)
		this->OnConnectionEstablishedDelegate.Broadcast();

		return true;
	}
}

void UCommunicationManager::Initialize(int Port, const FString& Address)
{
	this->ServerURL = Address + FString(":") + FString::FromInt(Port);
	Builder = new grpc::ServerBuilder();
	Builder->AddListeningPort(TCHAR_TO_UTF8(*ServerURL), grpc::InsecureServerCredentials());
}

