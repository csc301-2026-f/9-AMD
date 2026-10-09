// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "TrainingUtils/ArgBuilder.h"
#include "RPCServerSettings.generated.h"
/**
 * @brief A struct to hold settings relating to external communication (e.g. sockets)
 */
USTRUCT(BlueprintType)
struct SCHOLAPROTOBUF_API FRPCServerSettings
{
	GENERATED_BODY()

public:
	/** Bind address or hostname for the Python client to connect to. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Communicator Settings")
	FString Address = FString("127.0.0.1");

	/** TCP port for the gRPC / socket server. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Communicator Settings")
	int Port = 8000;

	/** Writes protocol URL and port into script argument keys consumed by Schola Python. */
	void GetArgs(FScriptArgBuilder& ArgBuilder) const
	{
		ArgBuilder.AddIntArg("port",this->Port);
		ArgBuilder.AddStringArg("url", this->Address);
	}
	
};