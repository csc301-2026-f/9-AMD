// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"

#include "TrainingUtils/ArgBuilder.h"
#include "TrainingSettings/TrainingSettings.h"
#include "RLlibIMPALASettings.generated.h"

/**
 * @brief A struct to hold IMPALA settings for an RLLib training script
 * @note This is a partial implementation of the IMPALA settings, and is not exhaustive
 */
USTRUCT(BlueprintType)
struct SCHOLATRAINING_API FRLlibIMPALASettings : public FTrainingSettings
{
	GENERATED_BODY()

public:
	/** Whether to enable V-trace in IMPALA. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IMPALA Settings")
    bool bVTrace = true;

    /** V-trace rho clipping threshold. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IMPALA Settings")
    float VTraceClipRhoThreshold = 1.0;

    /** V-trace policy-gradient rho clipping threshold. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "IMPALA Settings")
    float VTraceClipPGRhoThreshold = 1.0;

	/** Appends IMPALA-related CLI arguments to the training script builder. */
	void GenerateTrainingArgs( FScriptArgBuilder& ArgBuilder) const;

	virtual ~FRLlibIMPALASettings();
};
