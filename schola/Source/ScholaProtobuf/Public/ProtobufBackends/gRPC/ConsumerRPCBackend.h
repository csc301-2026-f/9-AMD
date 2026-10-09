// Copyright (c) 2023-2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once

#include "Common/LogSchola.h"
#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"
#include "Containers/Queue.h"
#include "RPCBackend.h"
#include "CallData.h"
#include "ProtobufBackends/ConsumerBackend.h"

/**
 * @brief Runnable worker that drains the gRPC completion queue and enqueues incoming requests.
 * @tparam ServiceType gRPC service type.
 * @tparam RequestType Protobuf request type.
 * @tparam ResponseType Protobuf response type.
 */
template <class ServiceType, typename RequestType, typename ResponseType>
class TConsumerRPCWorker : public FRunnable
{
private:
	/** CallData specialization used when interpreting completion-queue tags. */
	using CallData = TCallData<ServiceType, RequestType, ResponseType>;
	// CQueue owned by this workers parent
	ServerCompletionQueue* CQueue;

public:
	/** OS thread running Run() for completion-queue polling. */
	FRunnableThread*	Thread = nullptr;
	/** Fully received requests dequeued from CallData for Poll() consumers. */
	TQueue<RequestType> Requests = TQueue<RequestType>();

	/**
	 * @param[in] CQueue Completion queue to poll (owned by parent backend).
	 */
	TConsumerRPCWorker(ServerCompletionQueue* CQueue)
	{
		this->CQueue = CQueue;
	}

	~TConsumerRPCWorker()
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
		UE_LOGFMT(LogScholaCommunicator, Verbose, "TConsumerRPCWorker::Run(): Polling Thread Started");
		// This thread will loop through and fulfill promises etc on the exchange server
		void* tag = nullptr; // uniquely identifies a request.
		bool  ok = true;
		while (true)
		{
			// wait until some message is ready
			UE_LOGFMT(LogScholaCommunicator, VeryVerbose, "TConsumerRPCWorker::Run(): Waiting for Event on Polling queue");
			bool Status = CQueue->Next(&tag, &ok);
			if (!Status)
			{
				// Queue drained so we can exit
				UE_LOGFMT(LogScholaCommunicator, Verbose, "TConsumerRPCWorker::Run(): Polling Queue Drained and Shutdown");
				return -1;
			}
			else if (!ok)
			{
				UE_LOGFMT(LogScholaCommunicator, Verbose, "TConsumerRPCWorker::Run(): Invalid Event in Polling Completion Queue");
				// Clean up the message
				if (tag != nullptr)
				{
					CallData* CallDataPtr = static_cast<CallData*>(tag);
					CallDataPtr->CleanUp();
				}
				UE_LOGFMT(LogScholaCommunicator, Verbose, "TConsumerRPCWorker::Run(): Returning From Completion Queue");
			}
			else
			{
				// Normal event handling
				UE_LOGFMT(LogScholaCommunicator, VeryVerbose, "TConsumerRPCWorker::Run(): Queue had an Event!");
				CallData* CallDataPtr = static_cast<CallData*>(tag);
				
				if (CallDataPtr->IsReady())
				{
					UE_LOGFMT(LogScholaCommunicator, VeryVerbose, "TConsumerRPCWorker::Run(): Message Received on Poll!");
					Requests.Enqueue(CallDataPtr->GetRequest());
				}
				CallDataPtr->DoWork();
			}
		}
	}

	/**
	 * @brief Start the worker
	 */
	void Start()
	{
		UE_LOGFMT(LogScholaCommunicator, Verbose, "TConsumerRPCWorker::Start(): Starting Polling Worker");
		Thread = FRunnableThread::Create(this, TEXT("TConsumerRPCWorker"), 0, TPri_Normal);
	}

	/**
	 * @brief Shutdown the worker and it's associated completion queue
	 */
	virtual void Stop()
	{
		UE_LOGFMT(LogScholaCommunicator, Verbose, "TConsumerRPCWorker::Stop(): Shutting Down Polling Queue");
		CQueue->Shutdown();
		// Wait for the CQueue to Drain
		if (Thread != nullptr)
		{
			Thread->WaitForCompletion();
		}
	}

	/** FRunnable hook; unused for this worker. */
	virtual void Exit()
	{
		// Called on Completion so do nothing
	}
};

/**
 * @brief gRPC consumer backend: polls async RPCs and exposes dequeued requests to Unreal.
 */
template <class ServiceType, typename RequestType, typename ResponseType>
class TConsumerRPCBackend : public TRPCBackend<ServiceType, RequestType, ResponseType>, public IConsumerBackend<RequestType>
{
private:
	using CallData = TCallData<ServiceType, RequestType, ResponseType>;

	TConsumerRPCWorker<ServiceType, RequestType, ResponseType>* Worker;

	using gRPCBackend = TRPCBackend<ServiceType, RequestType, ResponseType>;

public:
	/**
	 * @param[in] TargetRPC Service method that receives incoming RPCs.
	 * @param[in] Service gRPC service instance.
	 * @param[in] CQueue Completion queue shared with the server.
	 */
	TConsumerRPCBackend(gRPCBackend::AsyncRPCHandle TargetRPC, std::shared_ptr<ServiceType> Service, std::unique_ptr<ServerCompletionQueue> CQueue)
		: gRPCBackend(TargetRPC, Service, std::move(CQueue))
	{
		this->Worker = new TConsumerRPCWorker<ServiceType, RequestType, ResponseType>(gRPCBackend::_CQueue.get());
	}

	~TConsumerRPCBackend()
	{
		Shutdown();
		delete this->Worker;
	}

	TOptional<const RequestType*> Poll() override
	{
		TRACE_CPUPROFILER_EVENT_SCOPE_STR("Schola: ConsumerRPCBackend Poll");
		if (Worker->Requests.IsEmpty())
		{
			return TOptional<const RequestType*>();
		}
		else
		{
			// Deque the front of the message queue
			RequestType RequestRef;
			Worker->Requests.Dequeue(RequestRef);
			return TOptional<const RequestType*>(new RequestType(RequestRef));
		}
	}

	virtual void Initialize(){};

	/** Seeds initial CallData and starts the completion-queue worker thread. */
	virtual void Start() override
	{
		new CallData(this->Service.get(), this->_CQueue.get(), gRPCBackend::TargetRPC, true);
		this->Worker->Start();
	}
	virtual void Establish(){};

	virtual void Shutdown() override
	{
		this->Worker->Stop();
	};

	virtual void Restart(){};

};
