// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"

#include "ExternalGymConnectorSettings.generated.h"
/**
 * @brief Configuration settings for external communication channels (e.g., sockets, HTTP).
 * @details Controls timeout and reliability settings for connectors that communicate with external processes.
 */
USTRUCT(BlueprintType)
struct SCHOLATRAINING_API FExternalGymConnectorSettings
{
	GENERATED_BODY()

public:
	/** Whether to use a timeout for decision requests from the external process. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (InlineEditConditionToggle), Category = "External Gym Connector Settings")
	bool bUseTimeout = true;

	/** The timeout duration in seconds for waiting on decision requests. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin=1, EditCondition="bUseTimeout"), Category = "External Gym Connector Settings")
	int Timeout = 30;
};


	