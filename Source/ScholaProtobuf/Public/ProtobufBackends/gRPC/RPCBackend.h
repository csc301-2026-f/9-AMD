// Copyright (c) 2023 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once

#include "HAL/Platform.h"

THIRD_PARTY_INCLUDES_START
#include "ScholaProtobufMacroGuardBegin.h"
#include "grpcpp/server.h"
#include "ScholaProtobufMacroGuardEnd.h"
THIRD_PARTY_INCLUDES_END

#include "CoreMinimal.h"

using grpc::Server;
using grpc::ServerCompletionQueue;
using grpc::ServerAsyncResponseWriter;

/**
 * @brief Base for async gRPC server backends sharing a completion queue and service instance.
 * @tparam ServiceType gRPC service implementation type.
 * @tparam RequestType Protobuf request message type for this RPC.
 * @tparam ResponseType Protobuf response message type for this RPC.
 */
template <class ServiceType, typename RequestType, typename ResponseType>
class TRPCBackend
{

protected:

	/** Server completion queue used for async RPC tags and polling. */
	std::unique_ptr<ServerCompletionQueue> _CQueue;
	/** gRPC service instance registered with the server. */
	std::shared_ptr<ServiceType>		   Service;
	/** gRPC server owning the service and queues. */
	std::unique_ptr<Server>				   Server;
	
	/** The signature of the handler for an asynchronous RPC, that is handled by this CallData. */
	using AsyncRPCHandle = void (ServiceType::*)(grpc::ServerContext* context,
		RequestType*												 request,
		ServerAsyncResponseWriter<ResponseType>*					 response,
		grpc::CompletionQueue*										 new_call_cq,
		ServerCompletionQueue*										 notification_cq,
		void*														 tag);
	
	/** Member pointer to the service method that begins this async RPC. */
	AsyncRPCHandle TargetRPC;

public:
	/**
	 * @param[in] TargetRPC Async RPC method on Service to invoke for new calls.
	 * @param[in] Service Shared service instance.
	 * @param[in] CQueue Completion queue for this backend (moved into member).
	 */
	TRPCBackend(AsyncRPCHandle TargetRPC, std::shared_ptr<ServiceType> Service, std::unique_ptr<ServerCompletionQueue> CQueue)
	{
		this->TargetRPC = TargetRPC;
		this->Service = Service;
		this->_CQueue = std::move(CQueue);
	}

	/** @param[in] CQueue Replacement completion queue (moved into member). */
	void SetCompletionQueue(std::unique_ptr<ServerCompletionQueue> CQueue)
	{
		this->_CQueue = std::move(CQueue);
	}
};