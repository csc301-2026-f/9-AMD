// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once

#include "ProtobufBackends/ProtobufBackend.h"
#include "ProtobufUtils/ProtobufDeserializer.h"
#include "ProtobufUtils/ProtobufSerializer.h"
/**
 * @brief A Generic Interface for any service that can be used to exchange messages of type In and Out asynchronously
 * @tparam In The type of message that will be input to this interface
 * @tparam Out The type of message that will be output by this interface
 */
template <typename In, typename Out>
class IExchangeBackend : public IProtobufBackend
{
public:
	/**
	 * @brief Initiate an Exchange with the Client.
	 * @return A future that will be fulfilled with the result of the exchange. The value ptr from the future is valid until the next time exchange is called.
	 */
	virtual TFuture<const In*> Receive() = 0;

	/**
	 * @brief Respond to a message from the client
	 * @param[in] Response The message to send to the client
	 */
	virtual void Respond(Out* Response) = 0;

	/**
	 * @brief Serialize an Unreal value to protobuf and send it as the exchange response.
	 * @tparam UnrealType Unreal struct type supported by ProtobufSerializer::ToProto. Must serialize to type Out.
	 * @param[in] InUnrealObject Value to convert and pass to Respond(Out*).
	 */
	template<typename UnrealType>
	void Respond(const UnrealType& InUnrealObject)
	{
		Out* OutPtr = new Out();
		ProtobufSerializer::ToProto(InUnrealObject, OutPtr);
		this->Respond(OutPtr);
	}

	/**
	 * @brief Reset the backend to clear any stale exchange state from previous connections
	 */
	virtual void Reset() = 0;

	/**
	 * @brief Do an exchange before converting the protomessage into the specified type
	 * @tparam T The type to deserialize the message into
	 * @return A proto message that has been deserialized from `In` into type `T`
	 */
	template <typename UnrealType>
	TFuture<UnrealType*> Receive()
	{
		// If T is not in our list of Deserializable Types this will cause a compilation error if this is the case
		TFuture<const In*> SerializedFuture = this->Receive();
		TPromise<UnrealType*>* DeserializedActionPromise = new TPromise<UnrealType*>();
		TFuture<UnrealType*>   FutureDeserializedAction = DeserializedActionPromise->GetFuture();

		SerializedFuture.Next(
			[DeserializedActionPromise](const In* Request) {
				// This will ensure that the type is deserializeable
				UnrealType* OutputMessage = new UnrealType();
				ProtobufDeserializer::FromProto(*Request, *OutputMessage);
				DeserializedActionPromise->SetValue(OutputMessage);
				delete DeserializedActionPromise;
			});

		return FutureDeserializedAction;
	}
};
