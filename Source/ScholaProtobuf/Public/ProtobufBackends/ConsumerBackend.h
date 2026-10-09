// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once
#include "ProtobufBackends/ProtobufBackend.h"
#include "ProtobufUtils/ProtobufDeserializer.h"

/**
 * @brief A Generic Interface for any service that can be used to receive messages of type T asynchronously via polling
 * @tparam T The type of message that will be output by this interface
 * 
 */
template <typename T>
class IConsumerBackend : public IProtobufBackend
{
public:
	
	/**
	 * @brief Poll the Backend for a message from the client
	 * @return An Empty optional if No messages have been received, a fulfilled one otherwise
	 * @note This function should be non-blocking
	 */
	virtual TOptional<const T*> Poll() = 0;

	/**
	 * @brief Poll the Backend for a message from the client and deserialize it into the specified type
	 * @tparam UnrealType The type to deserialize the message into
	 * @return An Empty optional if No messages have been received, a fulfilled one otherwise
	 */
	template <typename UnrealType>
	void Poll(TOptional<UnrealType>& OutDeserializedMessage)
	{
		TOptional<const T*> SerializedMessage = this->Poll();
		if (SerializedMessage.IsSet())
		{
			// Create a temporary value, deserialize into it, then assign to the optional
			UnrealType Value{};
			ProtobufDeserializer::FromProto(**SerializedMessage, Value);
			OutDeserializedMessage = MoveTemp(Value);
		}
		else
		{
			OutDeserializedMessage.Reset();
		}
	}
};