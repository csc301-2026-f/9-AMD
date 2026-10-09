// Copyright (c) 2023-2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once

#include "Common/LogSchola.h"
#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"
#include "Async/Future.h"
#include "Containers/Queue.h"
#include "./CallData.h"
#include "./RPCBackend.h"
#include "ProtobufUtils/ProtobufSerializer.h"
#include "ProtobufUtils/ProtobufDeserializer.h"
#include "ProtobufBackends/ExchangeBackend.h"


/**
 * @brief CallData for bidirectional exchange RPCs; fulfills a future when a request is ready.
 * @tparam ServiceType gRPC service type.
 * @tparam RequestType Protobuf request type.
 * @tparam ResponseType Protobuf response type.
 */
template <class ServiceType, typename RequestType, typename ResponseType>
class TExchangeCallData : public TCallData<ServiceType, RequestType, ResponseType>
{
protected:
	/** Promise completed when an incoming request is ready for Unreal to consume. */
	TPromise<const RequestType*> RequestPromise;
	/** Base CallData type for this RPC shape. */
	using CallData = TCallData<ServiceType, RequestType, ResponseType>;

public:
	/** True once FulfillRequestPromise has run for the current exchange. */
	bool bHasRequest = false;
	/** True for the initial CallData that bootstraps the RPC stream. */
	bool bIsFirst = false;

	/**
	 * @param[in] Service gRPC service instance.
	 * @param[in] CQueue Completion queue for async steps.
	 * @param[in] TargetRPC Service method pointer for this RPC.
	 * @param[in] bIsFirst Whether this is the bootstrap CallData instance.
	 */
	TExchangeCallData(ServiceType* Service, ServerCompletionQueue* CQueue, CallData::AsyncAPIHandler TargetRPC, bool bIsFirst = false)
		: CallData(Service, CQueue, TargetRPC, false, false), bIsFirst(bIsFirst)
	{
		// Note we disable autocreate here so that we can do some work first before creating the new calldata objects
	}

	/**
	 * @brief Get a future representing an eventual request from a client
	 * @return A Future that will be fulfilled once a RPC is initiated
	 */
	TFuture<const RequestType*> GetRequestFuture()
	{
		return RequestPromise.GetFuture();
	}

	/**
	 * @brief Send a request from a client to any consumers in Unreal
	 */
	void FulfillRequestPromise()
	{
		// Note there is an edge case that can cause nullptr exceptions, but doesn't occur in practice:
		//  1. we fulfill a request 2. server is shutdown 3. calldata is cleaned up by worker thread
		//  4. user access the pointer which now references nothing
		// One fix is to have this as a ptr instead of a member leaving it up to the user
		// to handle deallocation.
		//  Leaving this is okay for now because this only happens when the server shuts down which
		//  we don't promise to handle and continue.
		this->RequestPromise.EmplaceValue(&this->Request);
	}

	/**
	 * @brief Send a default value to the Unreal-side consumer.
	 * used to handle edge cases where two exchanges are made in a row
	 */
	void DefaultOnRequestPromise()
	{
		// make a default object of RequestType and pray
		this->RequestPromise.GetFuture().Reset();
		TSharedFuture<const RequestType*> OldFuture = this->RequestPromise.GetFuture().Share();

		this->RequestPromise.EmplaceValue(new RequestType());
	}
	/** Resets the pending request future to a default-constructed request after a failure path. */
	void Fail()
	{
		// We just die and then wait until we get vomitted onto the completion queue
		this->DefaultOnRequestPromise();
	}

	/**
	 * @brief Is this RPC ready to be completed.
	 */
	bool CanSubmit()
	{
		return this->Response != nullptr;
	}
};

// A debug int for tracking logs across Exchanges
static int ExchangeID = 0;

/**
 * @brief Runnable worker for exchange-style RPC completion-queue processing.
 * @tparam ServiceType gRPC service type.
 * @tparam RequestType Protobuf request type.
 * @tparam ResponseType Protobuf response type.
 */
template <class ServiceType, typename RequestType, typename ResponseType>
class ExchangeRPCWorker : public FRunnable
{
private:
	typedef TExchangeCallData<ServiceType, RequestType, ResponseType> _ExchCallData;
	// CQueue owned by this workers parent
	ServerCompletionQueue* CQueue;
	// The ID of this worker
	int LocalID;

public:
	/** OS thread running Run() for exchange completion events. */
	FRunnableThread* Thread = nullptr;

	/**
	 * @param[in] CQueue Completion queue to poll (owned by parent backend).
	 * @param[in] ID Worker id for logging.
	 */
	ExchangeRPCWorker(ServerCompletionQueue* CQueue, int ID)
	{
		LocalID = ID;
		this->CQueue = CQueue;
	}

	~ExchangeRPCWorker()
	{
		delete Thread;
	}
	/**
	 * @brief Initialize the worker
	 * @return True. Since initialization cannot fail
	 */
	virtual bool Init()
	{
		// Do Nothing here
		return true;
	}

	/**
	 * @brief This workers main method
	 * @return A status code representing the result
	 */
	virtual uint32 Run()
	{
		// This thread will loop through and fulfill promises etc on the exchange server
		void* tag = nullptr; // uniquely identifies a request.
		bool  ok = true;
		while (true)
		{
			
			bool Status = CQueue->Next(&tag, &ok);
			// Gotta check this way because if the queue was empty we also get a nonsense tag
			if (!Status)
			{
				// Queue drained so we can exit
				UE_LOGFMT(LogScholaCommunicator, Verbose, "ExchangeRPCWorker::Run(): Exchange Queue {0} Drained and Shutdown", LocalID);
				return -1;
			}
			else if (!ok)
			{
				// we can assume this since other events will have a tag
				// if we get nullptr and !ok then the Queue must be empty and therefore Status=False

				// This tag was cleanupable so clean it up
				if (tag != nullptr)
				{
					
					UE_LOGFMT(LogScholaCommunicator, Verbose, "ExchangeRPCWorker::Run(): Bad Event in Exchange Queue {0}, cleaning up the tag", LocalID);
					_ExchCallData* CallData = static_cast<_ExchCallData*>(tag);
					if (CallData->HasResponse())
					{
						UE_LOGFMT(LogScholaCommunicator, VeryVerbose, "ExchangeRPCWorker::Run(): Bad Event was Message: {0}", FString(CallData->GetRequest().DebugString().c_str()));
					}
					CallData->CleanUp();
				}
				else
				{
					UE_LOGFMT(LogScholaCommunicator, Warning, "ExchangeRPCWorker::Run(): Empty Event in Exchange Queue {0}. How did you get here? The Queue should be empty in this case.", LocalID);
				}
			}
			else
			{
				TRACE_CPUPROFILER_EVENT_SCOPE_STR("ScholaProtobuf: ExchangeRPCWorker Processing CallData");
				_ExchCallData* CallData = static_cast<_ExchCallData*>(tag);

				if (CallData->IsReady())
				{
					if (CallData->HasResponse())
					{
						UE_LOGFMT(LogScholaCommunicator, VeryVerbose, "ExchangeRPCWorker::Run(): Message in Exchange Queue {0}: {1}", LocalID, FString(CallData->GetRequest().DebugString().c_str()));
					}
					// fulfill the request promise but don't put it back on the queue
					// Note we will never double fullfill because we don't get back on the queue until we are out of process state
					CallData->FulfillRequestPromise();
					CallData->bHasRequest = true;
				}
				else
				{
					CallData->DoWork();
				}
			}
		}
	}

	/**
	 * @brief Start the worker
	 */
	void Start()
	{
		UE_LOGFMT(LogScholaCommunicator, Verbose, "ExchangeRPCWorker::Start(): Starting Exchange Worker {0}", LocalID);
		Thread = FRunnableThread::Create(this, TEXT("ExchangeRPCWorker"), 0, TPri_Normal);
	}

	/**
	 * @brief Shutdown the worker and it's associated completion queue
	 */
	virtual void Stop()
	{
		UE_LOGFMT(LogScholaCommunicator, Verbose, "ExchangeRPCWorker::Stop(): Shutting Down Exchange Queue {0}", LocalID);
		CQueue->Shutdown();
		// Wait for the CQueue to drain
		if (Thread != nullptr)
		{
			Thread->WaitForCompletion();
		}
	}

	/**
	 * @brief Unused. Called when the thread completes
	 */
	virtual void Exit()
	{
		// Called on Completion so do nothing
	}
};

/**
 * @brief gRPC exchange backend: async request/response pairing for bidirectional RPCs.
 */
template <class ServiceType, typename RequestType, typename ResponseType>
class TExchangeRPCBackend : public TRPCBackend<ServiceType, RequestType, ResponseType>, public IExchangeBackend<RequestType, ResponseType>
{
private:
	int																 LocalID = 0;
	using CallData = TExchangeCallData<ServiceType, RequestType, ResponseType>;
	// Note these are inverted since we are sending response, before the request arrives from gRPC perspective
	CallData*											   CurrExchange = nullptr;
	ExchangeRPCWorker<ServiceType, RequestType, ResponseType>* Worker;
	int														   MsgID = 0;
	using gRPCBackend = TRPCBackend<ServiceType, RequestType, ResponseType>;

public:
	/**
	 * @param[in] TargetRPC Service method used for exchange RPCs.
	 * @param[in] Service gRPC service instance.
	 * @param[in] CQueue Completion queue shared with the server.
	 */
	TExchangeRPCBackend(gRPCBackend::AsyncRPCHandle TargetRPC, std::shared_ptr<ServiceType> Service, std::unique_ptr<ServerCompletionQueue> CQueue)
		: gRPCBackend(TargetRPC, Service, std::move(CQueue))
	{
		LocalID = ExchangeID++;
		this->Worker = new ExchangeRPCWorker<ServiceType, RequestType, ResponseType>(gRPCBackend::_CQueue.get(), LocalID);
	}

	~TExchangeRPCBackend()
	{
		UE_LOGFMT(LogScholaCommunicator, Verbose, "TExchangeRPCBackend::~TExchangeRPCBackend(): Manually Deleting ExchangeRPC Backend {0}", LocalID);
		Shutdown();
		delete this->Worker;
	}

	TFuture<const RequestType*> Receive() override
	{
		TRACE_CPUPROFILER_EVENT_SCOPE_STR("ScholaProtobuf: ExchangeRPCBackend Receive");
		int TempId = MsgID++;
		// New CallData goes on a pending queue see: https://github.com/grpc/grpc/blob/v1.47.4/src/core/lib/surface/server.cc#L413
		checkf(CurrExchange == nullptr, TEXT("Existing Exchange needs to be completed before a new exchange can be started"));
		CallData* CallDataPtr = new CallData(this->Service.get(), this->_CQueue.get(), this->TargetRPC);
		CurrExchange = CallDataPtr;
		CallDataPtr->Id = TempId;
		CallDataPtr->Create();
		UE_LOGFMT(LogScholaCommunicator, VeryVerbose, "TExchangeRPCBackend::Receive(): Exchange {0}: Started Exchange {1}", LocalID, CurrExchange->Id);

		return CurrExchange->GetRequestFuture();
	}

	void Respond(ResponseType* Response) override
	{
		TRACE_CPUPROFILER_EVENT_SCOPE_STR("Schola: ExchangeRPCBackend Respond");
		assert(Service.Get() != nullptr);
		assert(CQueue.Get() != nullptr);
		checkf(CurrExchange != nullptr, TEXT("No Existing Exchange to Complete."));
		UE_LOGFMT(LogScholaCommunicator, VeryVerbose, "TExchangeRPCBackend::Respond(): Exchange {0}: Completed Exchange {1}", LocalID, CurrExchange->Id);
		CurrExchange->SetResponse(Response);
		CurrExchange->Submit();
		CurrExchange = nullptr;
	}

	virtual void Initialize() {};

	/** Starts the exchange completion-queue worker thread. */
	virtual void Start()
	{
		Worker->Start();
	}

	virtual void Establish() {};

	virtual void Shutdown() override
	{
		this->Worker->Stop();
		this->CurrExchange = nullptr;
	};

	virtual void Restart() {};

	virtual void Reset() override
	{
		// Clean up any stale exchange state from previous connection
		if (CurrExchange != nullptr)
		{
			UE_LOGFMT(LogScholaCommunicator, Verbose, "TExchangeRPCBackend::Reset(): Exchange {0}: Completing stale exchange on reset", LocalID);
			// Properly complete the pending exchange to avoid orphaning the CallData
			// This prevents the assertion failure when a new exchange is started
			Respond(new ResponseType());
		}
	}
};
