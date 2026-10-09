// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"
#include "LaunchableScript.generated.h"

/**
 * @brief Runnable task that executes a script in a background thread.
 * @details Handles script process management, including stdout/stderr pipe reading and logging.
 */
struct FLaunchableScriptRunnable : public FRunnable
{
	/**
	 * @brief Construct a runnable for the given script.
	 * @param[in] ScriptURL Path to the script to execute.
	 * @param[in] Args Command-line arguments for the script.
	 */
	FLaunchableScriptRunnable(FString ScriptURL, FString Args);

	/** Path to the script to execute. */
	FString		ScriptURL;
	/** Command-line arguments for the script. */
	FString		Args;
	/** Pipe for reading stderr from the script process. */
	void*		ReadStdErrPipe = nullptr;
	/** Pipe for writing stderr to the script process. */
	void*		WriteStdErrPipe = nullptr;
	/** Pipe for reading stdout from the script process. */
	void*		ReadStdOutPipe = nullptr;
	/** Pipe for writing stdout to the script process. */
	void*		WriteStdOutPipe = nullptr;
	/** Handle to the running script process. */
	FProcHandle ProcessHandle;

	/**
	 * @brief Initialize the runnable. Called once before Run().
	 * @return True if initialization succeeded.
	 */
	virtual bool   Init() override;

	/**
	 * @brief Main execution function that runs in the background thread.
	 * @return Exit code from the thread.
	 */
	virtual uint32 Run() override;

	/**
	 * @brief Called when the thread exits.
	 */
	virtual void   Exit() override;

	/**
	 * @brief Called to stop the runnable execution.
	 */
	virtual void   Stop() override;
};

/**
 * @brief A struct to hold the configuration of a launchable script.
 */
USTRUCT(BlueprintType)
struct SCHOLATRAINING_API FLaunchableScript
{
	GENERATED_BODY()

public:
	/** Static registry of active scripts for console command access */
	static TMap<FGuid, FLaunchableScript*> ActiveScripts;
	
	/** Unique identifier for this script instance */
	FGuid ScriptID;
	
	/** Display name for this script instance */
	FString ScriptInstanceName;
	/**
	 * @brief Construct a launchable script with no arguments or URL. Will fail to launch unless ScriptURL is set
	 */
	FLaunchableScript();

	~FLaunchableScript();

	/**
	 * @brief Construct a launchable script with a file URL
	 * @param[in] ScriptURL The URL of the script to launch
	 */
	FLaunchableScript(FString ScriptURL);

	/**
	 * @brief Construct a launchable script with a file URL and arguments
	 * @param[in] ScriptURL The URL of the script to launch
	 * @param[in] Args The arguments to pass to the script
	 */
	FLaunchableScript(FString ScriptURL, FString Args);

	/** A path to the script to be launched */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Networking URL")
	FString ScriptURL;

	/** The arguments to be passed to the script */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Networking URL")
	FString Args;

	/** Whether the background mirroring worker is active. */
	bool bIsMirroring;

	/** 
	 * @brief Append additional arguments to the script.
	 * @param[in] AdditionalArgs The arguments to append to the script.
	 */
	void AppendArgs(const FString& AdditionalArgs);

	/**
	 * @brief Launch the script in a background thread.
	 * @details Creates a new thread and process to execute the script asynchronously.
	 * The script's stdout and stderr are automatically logged.
	 */
	void LaunchScript();

	/**
	 * @brief Kill the currently running script process.
	 * @details Terminates the script process if it is currently running and cleans up resources.
	 * @return True if the process was successfully terminated or wasn't running, false on error.
	 */
	bool KillScript();

	/**
	 * @brief Static method to kill a script by its ID or name (for console commands).
	 * @param[in] IDOrName The ID (GUID string) or name of the script instance to kill.
	 * @return True if the script was found and killed, false otherwise.
	 */
	static bool KillScriptByIDOrName(const FString& IDOrName);

	/**
	 * @brief Static method to kill all running scripts (for console commands).
	 */
	static void KillAllScripts();

	/**
	 * @brief Static method to list all running scripts (for console commands).
	 */
	static void ListActiveScripts();

	/** The runnable task that executes the script. */
	FLaunchableScriptRunnable* Runnable = nullptr;
	/** The thread that runs the script. */
	FRunnableThread* Thread = nullptr;

private:
	/** Register this script in the active scripts registry */
	void RegisterScript();
	/** Unregister this script from the active scripts registry */
	void UnregisterScript();
};
