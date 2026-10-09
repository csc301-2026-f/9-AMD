// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once


/**
 * @brief An abstracted communication backend that can send string/byte messages and can either be polled for responses or do exchanges when it sends messages.
 * @note This isn't a UE interface because those don't have typical Object Oriented interface functionality which we want here
 */
class SCHOLAPROTOBUF_API IProtobufBackend
{
public:
	/**
	 * @brief Empty Desctructor explicitly defined to avoid C4265
	 */
	virtual ~IProtobufBackend(){};

	/**
	 * Perform any setup that happens prior to establishing any external connection.
	 * this could include opening a socket and waiting for a connection.
	 */
	virtual void Initialize() = 0;

	/**
	 * @brief Perform any setup that involves handshakes with the external communication (e.g. setting up serialization).
	 * Use after Initialize.
	 */
	virtual void Establish() = 0;

	/**
	 * @brief Close the external connection
	 */
	virtual void Shutdown() = 0;

	/**
	 * @brief Reset the Communication backend
	 */
	virtual void Restart() = 0;

	/**
	 * @brief Begin serving or polling after Initialize and Establish have completed.
	 * @details For gRPC backends this typically starts completion-queue worker threads.
	 */
	virtual void Start() = 0;
};
