// The below code is adapted from https://github.com/grpc/grpc/blob/master/examples/cpp/helloworld/greeter_async_server.cc
/*
 * Copyright 2015 gRPC authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
// Modifications Copyright (c) 2023 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once

#include "HAL/Platform.h"

THIRD_PARTY_INCLUDES_START
#include "ScholaProtobufMacroGuardBegin.h"
#include <grpc/grpc.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/server.h>
#include <grpcpp/server_context.h>
#include "ScholaProtobufMacroGuardEnd.h"
THIRD_PARTY_INCLUDES_END

#include "Common/LogSchola.h"

using grpc::Server;
using grpc::ServerAsyncResponseWriter;
using grpc::ServerCompletionQueue;
using grpc::ServerContext;
using grpc::Status;



/**
 * @brief A class that manages the state of a generic RPC call. DoWork() can be used in child threads to automatically progress the state of the call.
 * @tparam ServiceType The type of the service that this CallData is associated with
 * @tparam RequestType The type of the request message for the RPC, this is an incoming message to Unreal
 * @tparam ResponseType The type of the response message for the RPC, this is an outgoing message from Unreal
 */
template <class ServiceType, typename RequestType, typename ResponseType>
class TCallData
{

protected:

	/** The signature of the handler for an asynchronous RPC, that is handled by this CallData. */
	using AsyncAPIHandler = void (ServiceType::*)(grpc::ServerContext* context,
		RequestType*												 request,
		ServerAsyncResponseWriter<ResponseType>*					 response,
		grpc::CompletionQueue*										 new_call_cq,
		ServerCompletionQueue*										 notification_cq,
		void*														 tag);

	/** The Context of the Server that this CallData is associated with */
	ServerContext* SContext;

	/** An Object that can be used to send a response to the client */
	ServerAsyncResponseWriter<ResponseType> Responder;

	/** The incoming request message from the client */
	RequestType Request;

	/** The outgoing response message to the client */
	ResponseType* Response = nullptr;

	/** A method ptr to the type of Requests this CallData will be servicing */
	AsyncAPIHandler TargetRPC;

	/** An enum representing the possible states of the RPC */
	enum CallStatus
	{
		CREATE,
		PROCESS,
		FINISH
	};

	/** The current state of the RPC */
	CallStatus CallDataStatus;

	/** The service that this CallData is associated with */
	ServiceType* Service;

	/** The proiducer-consumer completion queue that this CallData is associated with */
	ServerCompletionQueue* CQueue;

	/** Variable defining whether we discard this CallData or replace it after serving one RPC. Default is to disgard */
	bool bReusable;

public:
	/** The unique ID of this CallData instance. Used for debugging, */
	int Id = 0;

	/**
	 * @brief Construct a new CallData object
	 * @param[in] Service The AsyncService that this CallData is associated with 
	 * @param[in] CQueue The completion queue that this CallData will tracked with
	 * @param[in] TargetRPC The method that this CallData will be servicing
	 * @param[in] bReusable Whether this CallData should be reused after serving one RPC
	 * @param[in] bAutoCreate Whether this CallData should automatically create itself, or it should happen later via Create()
	 */
	TCallData(ServiceType* Service, ServerCompletionQueue* CQueue, AsyncAPIHandler TargetRPC, bool bReusable = false, bool bAutoCreate = true)
		: SContext(new ServerContext()), Responder(SContext), CallDataStatus(CREATE), bReusable(bReusable)
	{
		this->TargetRPC = TargetRPC;
		this->CQueue = CQueue;
		this->Service = Service;
		if (bAutoCreate)
		{
			this->Create();
		}
	}

	/**
	 * @brief Create/setup the calldata instance. This registers the calldata as the tag to return when the TargetRPC is called.
	 */
	void Create()
	{
		TRACE_CPUPROFILER_EVENT_SCOPE_STR("ScholaProtobuf: CallData Create");
		if (CallDataStatus != CREATE)
		{
			UE_LOGFMT(
				LogScholaCommunicator,
				Fatal,
				"TCallData::Create(): invalid CallDataStatus; expected CREATE (got {0}).",
				static_cast<int>(CallDataStatus));
		}
		// Make this instance progress to the PROCESS state.
		CallDataStatus = PROCESS;
		// call the
		UE_LOGFMT(LogScholaCommunicator, VeryVerbose, "TCallData::Create(): Creating CallData");
		std::invoke(this->TargetRPC, this->Service, SContext, &Request, &Responder, CQueue, CQueue, this);
	}

	/**
	 * @brief Submit a response to the caller of the TargetRPC. This will put us back on the queue once the response finished processing.
	 */
	void Submit()
	{
		TRACE_CPUPROFILER_EVENT_SCOPE_STR("ScholaProtobuf: CallData Submit");
		if (CallDataStatus != PROCESS)
		{
			UE_LOGFMT(
				LogScholaCommunicator,
				Fatal,
				"TCallData::Submit(): invalid CallDataStatus; expected PROCESS (got {0}).",
				static_cast<int>(CallDataStatus));
		}
		CallDataStatus = FINISH;
		// create a default message if none has been set
		if (Response == nullptr)
		{
			Response = new ResponseType();
		}
		Responder.Finish(*Response, Status::OK, this);
	}

	/**
	 * @brief Reset the RPC state object so it can be reused again by another RPC in the future.
	 */
	void Reset()
	{
		if (CallDataStatus != FINISH)
		{
			UE_LOGFMT(
				LogScholaCommunicator,
				Fatal,
				"TCallData::Reset(): invalid CallDataStatus; expected FINISH before reset (got {0}).",
				static_cast<int>(CallDataStatus));
		}
		UE_LOGFMT(LogScholaCommunicator, VeryVerbose, "TCallData::Reset(): Resetting CallData");
		if (Response != nullptr)
		{
			delete this->Response;
		}
		this->Response = nullptr;
		// clean up the ServerContext
		// This is imperative since it can't be reused between RPC calls
		delete this->SContext;
		this->SContext = new ServerContext();
		// Reset the responder since it is only good for one response
		this->Responder = ServerAsyncResponseWriter<ResponseType>(this->SContext);
		Request.Clear();
		// Add ourselves back into the queue
		CallDataStatus = CREATE;
		this->Create();
	}

	/**
	 * @brief The RPC has been deleted so perform final cleanup
	 */
	void Finish()
	{
		// Once in the FINISH state, deallocate ourselves (CallData).
		if (CallDataStatus != FINISH)
		{
			UE_LOGFMT(
				LogScholaCommunicator,
				Fatal,
				"TCallData::Finish(): invalid CallDataStatus; expected FINISH before cleanup (got {0}).",
				static_cast<int>(CallDataStatus));
		}
		this->CleanUp();
	}

	/**
	 * @brief clean up, both this object and any other objects owned by it
	 */
	void CleanUp()
	{
		// Only use when draining the queue
		if (Response != nullptr)
		{
			delete this->Response;
		}

		delete this->SContext;
		delete this;
	}

	/**
	 * @brief Get the request associated with this RPC. May be empty if the RPC is not ready
	 * @return A reference to the RPC Request Message from the client
	 */
	const RequestType& GetRequest()
	{
		return Request;
	}

	/**
	 * @brief Get a Response message that can be mutated in place
	 * @return A pointer to a response message owned by this object
	 */
	ResponseType* GetMutableResponse()
	{
		this->Response = new ResponseType();
		return this->Response;
	}

	/**
	 * @brief Set the response message.
	 * @param[in] NewResponse The new Response message to associate with this call. 
	 * @note Caller must relinquish ownership of the response message
	 */
	void SetResponse(ResponseType* NewResponse)
	{
		TRACE_CPUPROFILER_EVENT_SCOPE_STR("ScholaProtobuf: CallData SetResponse");
		if (this->Response != nullptr)
		{
			delete this->Response;
		}
		this->Response = NewResponse;
	}

	/**
	 * @brief Has the response message been set already.
	 * @return true iff the response message has been set
	 */
	inline bool HasResponse()
	{
		return Response != nullptr;
	}

	/**
	 * @brief Returns whether the RPC is ready for processing. Specifically, does the RPC have a Request?
	 * @returns true iff this RPC has a request but has not submitted a response
	 */
	inline bool IsReady()
	{
		return CallDataStatus == CallStatus::PROCESS;
	}

	/**
	 * @brief Progress the state of the RPC. Call this on a child process to handle automatically setting up and completing gRPC calls.
	 */
	void DoWork()
	{

		if (CallDataStatus == CallStatus::CREATE)
		{
			this->Create();
		}
		else if (CallDataStatus == CallStatus::PROCESS)
		{

			this->Submit();
		}
		else
		{
			if (!bReusable)
			{
				this->Finish();
			}
			else
			{
				this->Reset();
			}
		}
	}
};