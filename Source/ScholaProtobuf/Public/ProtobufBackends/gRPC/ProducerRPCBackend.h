// Copyright (c) 2023-2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once

#include "Common/LogSchola.h"
#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"
#include "./CallData.h"
#include "./RPCBackend.h"
#include "ProtobufBackends/ProducerBackend.h"

// A debug int for tracking logs across Exchanges
static int ProducerID = 0;

/**
 * @brief Runnable worker that drives async response writes on the producer completion queue.
 */
template <class ServiceType, typename RequestType, typename ResponseType>
class ProducerRPCWorker : public FRunnable
{
private:
	typedef TCallData<ServiceType, RequestType, ResponseType> CallData;
	// CQueue owned by this workers parent
	ServerCompletionQueue* CQueue;
	// The ID of this worker
	int LocalID;

public:
	/** OS thread running Run() for producer completion events. */
	FRunnableThread* Thread = nullptr;

	/**
	 * @param[in] CQueue Completion queue to poll (owned by parent backend).
	 * @param[in] ID Worker id for logging.
	 */
	ProducerRPCWorker(ServerCompletionQueue* CQueue, int ID)
	{
		LocalID = ID;
		this->CQueue = CQueue;
	}

	~ProducerRPCWorker()
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
		UE_LOGFMT(LogScholaCommunicator, Verbose, "ProducerRPCWorker::Run(): Producer Thread {0} Started", LocalID);
		void* tag = nullptr; // uniquely identifies a request.
		bool  ok = true;
		while (true)
		{
			UE_LOGFMT(LogScholaCommunicator, VeryVerbose, "ProducerRPCWorker::Run(): Waiting for Event on Producer queue {0}", LocalID);
			bool Status = CQueue->Next(&tag, &ok);
			// Gotta check this way because if the queue was empty we also get a nonsense tag
			if (!Status)
			{
				// Queue drained so we can exit
				UE_LOGFMT(LogScholaCommunicator, Verbose, "ProducerRPCWorker::Run(): Producer Queue {0} Drained and Shutdown", LocalID);
				return -1;
			}
			else if (!ok)
			{
				// we can assume this since other events will have a tag
				// if we get nullptr and !ok then the Queue must be empty and therefore Status=False
				UE_LOGFMT(LogScholaCommunicator, Verbose, "ProducerRPCWorker::Run(): Invalid Event in Producer {0} Completion Queue", LocalID);
				// This tag was cleanupable so clean it up
				if (tag != nullptr)
				{
					CallData* CallDataPtr = static_cast<CallData*>(tag);
					CallDataPtr->CleanUp();
				}
			}
			else
			{
				CallData* CallDataPtr = static_cast<CallData*>(tag);
				CallDataPtr->DoWork();
			}
		}
	}

	/**
	 * @brief Start the worker
	 */
	void Start()
	{
		UE_LOGFMT(LogScholaCommunicator, Verbose, "ProducerRPCWorker::Start(): Starting Producer Worker {0}", LocalID);
		Thread = FRunnableThread::Create(this, TEXT("ProducerRPCWorker"), 0, TPri_Normal);
	}

	/**
	 * @brief Shutdown the worker and it's associated completion queue
	 */
	virtual void Stop()
	{
		UE_LOGFMT(LogScholaCommunicator, Verbose, "ProducerRPCWorker::Stop(): Shutting Down Producer Queue {0}", LocalID);
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
 * @brief gRPC producer backend: sends responses to the clients, discards their requests
 */
template <class ServiceType, typename RequestType, typename ResponseType>
class TProducerRPCBackend : public TRPCBackend<ServiceType, RequestType, ResponseType>, public IProducerBackend<ResponseType>
{
private:
	int														 LocalID = 0;
	int														 MsgID = 0;
	typedef TCallData<ServiceType, RequestType, ResponseType> CallData;
	// Note these are inverted since we are sending response, before the request arrives from gRPC perspective
	ProducerRPCWorker<ServiceType, RequestType, ResponseType>* Worker;
	using gRPCBackend = TRPCBackend<ServiceType, RequestType, ResponseType>;

public:
	/**
	 * @param[in] TargetRPC Service method used for outbound async responses.
	 * @param[in] Service gRPC service instance.
	 * @param[in] CQueue Completion queue shared with the server.
	 */
	TProducerRPCBackend(gRPCBackend::AsyncRPCHandle TargetRPC, std::shared_ptr<ServiceType> Service, std::unique_ptr<ServerCompletionQueue> CQueue)
		: gRPCBackend(TargetRPC, Service, std::move(CQueue))
	{
		LocalID = ExchangeID++;
		this->Worker = new ProducerRPCWorker<ServiceType, RequestType, ResponseType>(gRPCBackend::_CQueue.get(), LocalID);
	}

	~TProducerRPCBackend()
	{
		UE_LOGFMT(LogScholaCommunicator, Verbose, "TProducerRPCBackend::~TProducerRPCBackend(): Manually Deleting ProducerRPC Backend {0}", LocalID);
		Shutdown();
		delete this->Worker;
	}

	void Publish(ResponseType* Response) override
	{
		TRACE_CPUPROFILER_EVENT_SCOPE_STR("Schola: ProducerRPCBackend Publish");
		// Defensive check: ensure service and completion queue are valid before attempting publish
		if (!this->Service || !this->_CQueue)
		{
			UE_LOGFMT(LogScholaCommunicator, Error, "TProducerRPCBackend::Publish(): Backend {0}: Cannot publish - service or completion queue is null", LocalID);
			delete Response; // Clean up the response to avoid memory leak
			return;
		}
		
		CallData* CallDataPtr = new CallData(this->Service.get(), this->_CQueue.get(), this->TargetRPC, false, false);
		CallDataPtr->SetResponse(Response);
		int TempID = MsgID++;
		CallDataPtr->Id = TempID;
		CallDataPtr->Create();
	}

	virtual void Initialize() {};

	/** Starts the producer completion-queue worker thread. */
	virtual void Start()
	{
		Worker->Start();
	}

	virtual void Establish() {};

	virtual void Shutdown() override
	{
		this->Worker->Stop();
	};

	virtual void Restart() {};
};