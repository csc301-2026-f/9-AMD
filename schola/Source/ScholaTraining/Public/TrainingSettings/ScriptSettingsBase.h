// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "TrainingUtils/LaunchableScript.h"
#include "TrainingUtils/ArgBuilder.h"
#include "ScriptSettingsBase.generated.h"

/**
 * @brief The type of a training script.
 */
UENUM(BlueprintType)
enum class EScriptType : uint8
{
	Python UMETA(DisplayName = "Python"),
	Other  UMETA(DisplayName = "Other"),
};

/**
 * @brief The type of a python environment.
 */
UENUM(BlueprintType)
enum class EPythonEnvironmentType : uint8
{
	/** The built-in python interpreter from Unreal Engine */
	BuiltIn UMETA(DisplayName = "Built-in Python Env"),
	/** The default system python */
	SystemPath UMETA(DisplayName = "System PATH Python Env"),
	/** A conda environment with a specified name */
	Conda	UMETA(DisplayName = "Conda Env"),
	/** A virtual environment with a specified path */
	VEnv	UMETA(DisplayName = "Custom Python Path"),
};

/**
 * @brief Base class for all script settings (Training, Imitation, etc).
 * @details Contains common functionality for Python environment management
 * and script launching that is shared across different script types.
 */
USTRUCT()
struct SCHOLATRAINING_API FScriptSettingsBase
{
	GENERATED_BODY()

public:
	
	/** The type of the script to run (e.g. Python, or Other CLI script) */
	UPROPERTY(Config, EditAnywhere, Category= "Script Settings")
	EScriptType ScriptType = EScriptType::Python;

	/** The Python environment to use when running a python script */
	UPROPERTY(Config, EditAnywhere, meta = (EditCondition = "ScriptType==EScriptType::Python", EditConditionHides), Category = "Script Settings|Python Env Settings")
	EPythonEnvironmentType EnvType = EPythonEnvironmentType::BuiltIn;

	/** The name of the conda environment to run the python script in */
	UPROPERTY(Config, EditAnywhere, meta = (EditCondition = "EnvType==EPythonEnvironmentType::Conda && ScriptType==EScriptType::Python", EditConditionHides), Category = "Script Settings|Python Env Settings")
	FString CondaEnvName;

	/** The path to a python executable if using a non-default executable */
	UPROPERTY(Config, EditAnywhere, meta = (EditCondition = "EnvType==EPythonEnvironmentType::VEnv && ScriptType==EScriptType::Python", EditConditionHides), Category = "Script Settings|Python Env Settings")
	FFilePath CustomPythonPath;

	/**
	 * @brief Get a configured launchable script ready to execute.
	 * @return A launchable script struct with path set.
	 */
	FLaunchableScript GetLaunchableScript() const;

	/**
	 * @brief Get the path to the built-in Unreal Engine Python interpreter.
	 * @return Path to the Python executable.
	 */
	FString GetBuiltInPythonPath() const;

	/**
	 * @brief Verify that Schola is installed in the Python environment.
	 * @details Logs warnings if Schola is not found.
	 */
	void EnsureScholaIsInstalled() const;

	/**
	 * @brief Get the file system path to the script to run.
	 * @details This is a pure virtual function that must be implemented by derived classes.
	 * @return The path to the script executable/file.
	 */
	virtual FString GetScriptPath() const { return FString(); };

	/**
	 * @brief Get the arguments for the script to run.
	 * @details This is a pure virtual function that must be implemented by derived classes.
	 * @param[in] InArgBuilder The argument builder to populate.
	 * @return The generated argument string.
	 */
	virtual void GetArgs(FScriptArgBuilder& InArgBuilder) const {};

	/**
	 * @brief Get the arguments for the script to run.
	 * @details This is a pure virtual function that must be implemented by derived classes.
	 * @return The generated argument string.
	 */
	FString GetArgs() const;

	/**
	 * @brief Virtual destructor.
	 */
	virtual ~FScriptSettingsBase();
};
