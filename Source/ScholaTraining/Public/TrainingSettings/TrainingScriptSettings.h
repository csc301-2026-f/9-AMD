// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "TrainingSettings/ScriptSettingsBase.h"
#include "TrainingSettings/TrainingSettings.h"
#include "TrainingSettings/StableBaselines/SB3TrainingSettings.h"
#include "TrainingSettings/Ray/RLlibTrainingSettings.h"
#include "TrainingSettings/Custom/CustomTrainingSettings.h"
#include "Interfaces/IPluginManager.h"
#include "TrainingUtils/LaunchableScript.h"
#include "TrainingUtils/ArgBuilder.h"
#include "TrainingScriptSettings.generated.h"

FString WithQuotes(FString Input);

/**
 * @brief The type of a python training script to use.
 */
UENUM(BlueprintType)
enum class EPythonScript : uint8
{
	/** use the SB3 launch script included with Schola */
	SB3	   UMETA(DisplayName = "Builtin SB3 Training Script"),
	/** use the RLlib launch script included with Schola */
	RLLIB  UMETA(DisplayName = "Builtin RLlib Training Script"),
	/** use a custom launch script set by the user*/
	Custom UMETA(DisplayName = "Custom Training Script"),
};


/**
 * @brief All settings for autolaunching a training script when starting the game.
 */
USTRUCT(BlueprintType)
struct SCHOLATRAINING_API FTrainingScriptSettings : public FScriptSettingsBase
{
	GENERATED_BODY()

public:

	/** The type of python script to run, options are scripts builtin to Schola, or a user provided script */
	UPROPERTY(Config, EditAnywhere, meta = (EditCondition = "ScriptType==EScriptType::Python", EditConditionHides), Category = "Script Settings|Python Script")
	EPythonScript PythonScriptType = EPythonScript::SB3;

	/** Settings to use when running a custom python script */
	UPROPERTY(Config, EditAnywhere, meta = (EditCondition = "ScriptType==EScriptType::Python && PythonScriptType==EPythonScript::Custom", EditConditionHides), Category = "Script Settings|Python Script")
	FCustomTrainingSettings CustomPythonScriptSettings;

	/** Settings to use when running a custom script */
	UPROPERTY(Config, EditAnywhere, meta = (EditCondition = "ScriptType==EScriptType::Other", EditConditionHides), Category = "Script Settings|Custom Script")
	FCustomTrainingSettings CustomScriptSettings;

	/** Settings to use when running the builtin SB3 script */
	UPROPERTY(Config, EditAnywhere, meta = (EditCondition = "ScriptType==EScriptType::Python && PythonScriptType==EPythonScript::SB3", EditConditionHides, DisplayName = "Builtin SB3 Settings"), Category = "Script Settings|Python Script")
	FSB3TrainingSettings SB3Settings;

	/** Settings to use when running the builtin RLlib script */
	UPROPERTY(Config, EditAnywhere, meta = (EditCondition = "ScriptType==EScriptType::Python && PythonScriptType==EPythonScript::RLlib", EditConditionHides, DisplayName = "Builtin RLlib Settings"), Category = "Script Settings|Python Script")
	FRLlibTrainingSettings RLlibSettings;

	virtual void GetArgs(FScriptArgBuilder& InArgBuilder) const override;

	/**
	 * @brief Get the file system path to the script to run.
	 * @return The path to the script executable/file.
	 */
	virtual FString GetScriptPath() const override;

	/**
	 * @brief Virtual destructor.
	 */
	virtual ~FTrainingScriptSettings();
};
