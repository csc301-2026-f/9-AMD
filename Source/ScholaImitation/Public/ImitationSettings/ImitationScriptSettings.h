// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "TrainingSettings/ScriptSettingsBase.h"
#include "ImitationSettings/ImitationSettings.h"
#include "ImitationSettings/Minari/MinariCollectionSettings.h"
#include "TrainingUtils/ArgBuilder.h"
#include "TrainingUtils/LaunchableScript.h"
#include "ImitationScriptSettings.generated.h"


/**
 * @brief Top-level configuration struct for imitation learning script settings.
 * @details Contains all settings necessary to launch and configure an imitation learning session.
 * This provides a framework-agnostic interface that can accommodate different imitation learning
 * frameworks (e.g., Minari, custom tools) similar to how training supports SB3, RLlib, etc.
 * Inherits Python environment management from FScriptSettingsBase.
 */
USTRUCT(BlueprintType)
struct SCHOLAIMITATION_API FImitationScriptSettings : public FScriptSettingsBase
{
	GENERATED_BODY()

public:
	
	/** Settings to use when running the Minari collection framework */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "Minari Settings"), Category = "Imitation Script Settings")
	FMinariCollectionSettings MinariSettings;

	// Future frameworks can be added here, e.g.:
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "Framework==EImitationFramework::CustomFramework", EditConditionHides), Category = "Imitation Script Settings")
	// FCustomFrameworkSettings CustomFrameworkSettings;

	/**
	 * @brief Generate script arguments using the provided argument builder.
	 * @param[in,out] InArgBuilder The argument builder to populate.
	 * @return The generated argument string.
	 */
	virtual void GetArgs(FScriptArgBuilder& InArgBuilder) const override;


	/**
	 * @brief Get the file system path to the imitation learning script.
	 * @return The path to the script file.
	 */
	virtual FString GetScriptPath() const override;

	/**
	 * @brief Virtual destructor.
	 */
	virtual ~FImitationScriptSettings();
};
