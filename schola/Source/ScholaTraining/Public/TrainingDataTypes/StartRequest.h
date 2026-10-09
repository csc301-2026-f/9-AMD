// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"

#include "GymConnectors/AutoResetTypeEnum.h"
#include "StartRequest.generated.h"


/**
 * @brief A struct representing a message indicating that the connector should start.
 */
USTRUCT()
struct SCHOLATRAINING_API FStartRequest
{

	GENERATED_BODY()

public:
	/** The AutoReset Type Received from Python. */
	UPROPERTY(EditAnywhere, Category = "Schola|Training")
	EAutoResetType AutoResetType = EAutoResetType::SameStep; // Default to SameStep

	/**
	 * @brief Create a new empty FStartRequest.
	 */
	FStartRequest(EAutoResetType InAutoResetType)
		: AutoResetType(InAutoResetType){};

	FStartRequest() {};

};
