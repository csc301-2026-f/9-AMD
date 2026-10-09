 // Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "TrainingUtils/LaunchableScript.h"
#include "Async/Async.h"
#include "HAL/IConsoleManager.h"
#include "LogScholaTraining.h"
#include "Misc/Paths.h"

// Initialize static map
TMap<FGuid, FLaunchableScript*> FLaunchableScript::ActiveScripts;

// Console command to kill a script by ID or name
static FAutoConsoleCommand KillScriptCommand(
	TEXT("Script.Kill"),
	TEXT("Kill a running script by ID or name. Usage: Script.Kill <ScriptID or ScriptName>"),
	FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
	{
		if (Args.Num() == 0)
		{
			UE_LOGFMT(LogScholaTraining, Warning, "FLaunchableScript (Script.Kill): Usage: Script.Kill <ScriptID or ScriptName>");
			FLaunchableScript::ListActiveScripts();
			return;
		}
		FLaunchableScript::KillScriptByIDOrName(Args[0]);
	})
);

// Console command to kill all scripts
static FAutoConsoleCommand KillAllScriptsCommand(
	TEXT("Script.KillAll"),
	TEXT("Kill all running scripts"),
	FConsoleCommandDelegate::CreateLambda([]()
	{
		FLaunchableScript::KillAllScripts();
	})
);

// Console command to list active scripts
static FAutoConsoleCommand ListScriptsCommand(
	TEXT("Script.List"),
	TEXT("List all currently running scripts"),
	FConsoleCommandDelegate::CreateLambda([]()
	{
		FLaunchableScript::ListActiveScripts();
	})
);

FLaunchableScript::FLaunchableScript()
{
	this->ScriptURL = FString("");
	this->Args = FString("");
}

FLaunchableScript::FLaunchableScript(FString ScriptURL)
{
	this->ScriptURL = ScriptURL;
	this->Args = FString("");
}

FLaunchableScript::FLaunchableScript(FString ScriptURL, FString Args)
{
	this->ScriptURL = ScriptURL;
	this->Args = Args;
}

void FLaunchableScript::AppendArgs(const FString& AdditionalArgs)
{
	this->Args += (FString(" ") + AdditionalArgs);
}

void FLaunchableScript::LaunchScript()
{
	UE_LOGFMT(LogScholaTraining, Log, "FLaunchableScript::LaunchScript(): Launching {0} {1}", ScriptURL, Args);
	if(this->Thread)
	{
		UE_LOGFMT(LogScholaTraining, Warning, "FLaunchableScript::LaunchScript(): Script already running");
		return;
	}
	
	// Generate unique ID for this script instance
	ScriptID = FGuid::NewGuid();
	
	// Generate display name if not set
	if (ScriptInstanceName.IsEmpty())
	{
		ScriptInstanceName = FPaths::GetBaseFilename(ScriptURL);
	}
	
	this->Runnable = new FLaunchableScriptRunnable(ScriptURL, Args);
	this->Thread = FRunnableThread::Create(this->Runnable, TEXT("LaunchableScriptRunnable"));
	
	// Register this script for console command access
	RegisterScript();
	
	UE_LOGFMT(LogScholaTraining, Log, "FLaunchableScript::LaunchScript(): Script registered with ID: {0}, Name: {1}", ScriptID.ToString(), ScriptInstanceName);
}

bool FLaunchableScript::KillScript()
{
	if (!this->Thread || !this->Runnable)
	{
		UE_LOGFMT(LogScholaTraining, Warning, "FLaunchableScript::KillScript(): No script is currently running");
		return true; // No script to kill is considered success
	}

	UE_LOGFMT(LogScholaTraining, Log, "FLaunchableScript::KillScript(): Killing script {0} (ID: {1}, Name: {2})", ScriptURL, ScriptID.ToString(), ScriptInstanceName);

	// Unregister from active scripts
	UnregisterScript();

	// Kill the thread and wait for it to exit
	this->Thread->Kill(true);
	delete this->Thread;
	this->Thread = nullptr;

	delete this->Runnable;
	this->Runnable = nullptr;

	UE_LOGFMT(LogScholaTraining, Log, "FLaunchableScript::KillScript(): Script killed successfully");
	return true;
}

FLaunchableScript::~FLaunchableScript()
{
	// Unregister from active scripts
	UnregisterScript();
	
	if (this->Thread && this->Runnable)
	{
		//Kill and wait for thread to exit
		this->Thread->Kill(true);
		delete this->Thread;
		this->Thread = nullptr;

		delete this->Runnable;
		this->Runnable = nullptr;
	}
}

//FLaunchableScriptRunnable


FLaunchableScriptRunnable::FLaunchableScriptRunnable(FString ScriptURL, FString Args)
{
	this->ScriptURL = ScriptURL;
	this->Args = Args;
	
}

bool FLaunchableScriptRunnable::Init()
{
	return true;
}

uint32 FLaunchableScriptRunnable::Run()
{
	FPlatformProcess::CreatePipe(this->ReadStdOutPipe, this->WriteStdOutPipe);
	FPlatformProcess::CreatePipe(this->ReadStdErrPipe, this->WriteStdErrPipe);
	this->ProcessHandle = FPlatformProcess::CreateProc(*this->ScriptURL, *this->Args, false, false, false, nullptr, 0, nullptr, WriteStdOutPipe, nullptr, WriteStdErrPipe);
	//this->ProcessHandle = FPlatformProcess::CreateProc(*this->ScriptURL, *this->Args, false, false, false, nullptr, 0, nullptr, nullptr);

	// If process launched successfully, spawn an async worker to mirror stdout/stderr to the Unreal log.
	if (this->ProcessHandle.IsValid())
	{

		FString StdOut;
		FString StdErr;

		// Mirror while process is running
		while (FPlatformProcess::IsProcRunning(this->ProcessHandle))
		{
			FPlatformProcess::Sleep(0.1f);

			StdOut = FPlatformProcess::ReadPipe(this->ReadStdOutPipe);
			if (!StdOut.IsEmpty())
			{
				//TODO make this print out lines one by one
				UE_LOGFMT(LogScholaTraining, Log, "FLaunchableScriptRunnable::Run(): {0}", StdOut);
			}
			StdErr = FPlatformProcess::ReadPipe(this->ReadStdErrPipe);
			if (!StdErr.IsEmpty())
			{
				// TODO make this print out lines one by one
				UE_LOGFMT(LogScholaTraining, Log, "FLaunchableScriptRunnable::Run(): {0}", StdErr);
			}
		}
		
		// Drain remaining stdout after exit
		StdOut = FPlatformProcess::ReadPipe(this->ReadStdOutPipe);
		while (!StdOut.IsEmpty())
		{
			UE_LOGFMT(LogScholaTraining, Log, "FLaunchableScriptRunnable::Run():\n{0}", StdOut);
			StdOut = FPlatformProcess::ReadPipe(this->ReadStdOutPipe);
		}
		// Drain remaining stderr after exit
		StdErr = FPlatformProcess::ReadPipe(this->ReadStdErrPipe);
		while (!StdErr.IsEmpty())
		{
			UE_LOGFMT(LogScholaTraining, Log, "FLaunchableScriptRunnable::Run():\n{0}", StdErr);
			StdErr = FPlatformProcess::ReadPipe(this->ReadStdErrPipe);
		}
	}
	return 0;
}

void FLaunchableScriptRunnable::Exit()
{
	int32 ReturnCode;
	FPlatformProcess::GetProcReturnCode(this->ProcessHandle, &ReturnCode);
	if (ReturnCode != 0)
	{
		UE_LOGFMT(LogScholaTraining, Error, "FLaunchableScriptRunnable::Exit(): Script {0} exited with code {1}", this->ScriptURL, ReturnCode);
	}

	FPlatformProcess::CloseProc(this->ProcessHandle);
	FPlatformProcess::ClosePipe(this->ReadStdOutPipe, this->WriteStdOutPipe);
	FPlatformProcess::ClosePipe(this->ReadStdErrPipe, this->WriteStdErrPipe);

	this->ProcessHandle.Reset();
}

void FLaunchableScriptRunnable::Stop()
{
	FPlatformProcess::TerminateProc(this->ProcessHandle, true);
}

// Static methods for console command support

bool FLaunchableScript::KillScriptByIDOrName(const FString& IDOrName)
{
	// First, try to parse as GUID
	FGuid ParsedGuid;
	if (FGuid::Parse(IDOrName, ParsedGuid))
	{
		FLaunchableScript** ScriptPtr = ActiveScripts.Find(ParsedGuid);
		if (ScriptPtr && *ScriptPtr)
		{
			UE_LOGFMT(LogScholaTraining, Log, "FLaunchableScript::KillScriptByIDOrName(): Killing script by ID: {0}", IDOrName);
			(*ScriptPtr)->KillScript();
			return true;
		}
	}
	
	// If not found by ID, search by name
	for (const auto& Pair : ActiveScripts)
	{
		if (Pair.Value && Pair.Value->ScriptInstanceName.Equals(IDOrName, ESearchCase::IgnoreCase))
		{
			UE_LOGFMT(LogScholaTraining, Log, "FLaunchableScript::KillScriptByIDOrName(): Killing script by name: {0}", IDOrName);
			Pair.Value->KillScript();
			return true;
		}
	}
	
	UE_LOGFMT(LogScholaTraining, Warning, "FLaunchableScript::KillScriptByIDOrName(): Script not found: {0}", IDOrName);
	ListActiveScripts();
	return false;
}

void FLaunchableScript::KillAllScripts()
{
	if (ActiveScripts.Num() == 0)
	{
		UE_LOGFMT(LogScholaTraining, Log, "FLaunchableScript::KillAllScripts(): No scripts are currently running");
		return;
	}

	UE_LOGFMT(LogScholaTraining, Log, "FLaunchableScript::KillAllScripts(): Killing all {0} active scripts...", ActiveScripts.Num());
	
	// Create a copy of the keys to avoid iterator invalidation
	TArray<FGuid> ScriptIDs;
	ActiveScripts.GetKeys(ScriptIDs);
	
	for (const FGuid& ID : ScriptIDs)
	{
		FLaunchableScript** ScriptPtr = ActiveScripts.Find(ID);
		if (ScriptPtr && *ScriptPtr)
		{
			(*ScriptPtr)->KillScript();
		}
	}
	
	UE_LOGFMT(LogScholaTraining, Log, "FLaunchableScript::KillAllScripts(): All scripts killed");
}

void FLaunchableScript::ListActiveScripts()
{
	if (ActiveScripts.Num() == 0)
	{
		UE_LOGFMT(LogScholaTraining, Log, "FLaunchableScript::ListActiveScripts(): No scripts are currently running");
		return;
	}

	UE_LOGFMT(LogScholaTraining, Log, "FLaunchableScript::ListActiveScripts(): Active Scripts ({0}):", ActiveScripts.Num());
	for (const auto& Pair : ActiveScripts)
	{
		FLaunchableScript* Script = Pair.Value;
		if (Script)
		{
			UE_LOGFMT(LogScholaTraining, Log, "FLaunchableScript::ListActiveScripts():   - Name: {0} | ID: {1} | Script: {2} {3}",
				Script->ScriptInstanceName,
				Pair.Key.ToString(),
				Script->ScriptURL,
				Script->Args);
		}
	}
}

// Private methods for registry management

void FLaunchableScript::RegisterScript()
{
	if (ScriptID.IsValid())
	{
		ActiveScripts.Add(ScriptID, this);
		UE_LOGFMT(LogScholaTraining, Verbose, "FLaunchableScript::RegisterScript(): Registered script instance: {0} (ID: {1})", ScriptInstanceName, ScriptID.ToString());
	}
}

void FLaunchableScript::UnregisterScript()
{
	if (ScriptID.IsValid() && ActiveScripts.Contains(ScriptID))
	{
		ActiveScripts.Remove(ScriptID);
		UE_LOGFMT(LogScholaTraining, Verbose, "FLaunchableScript::UnregisterScript(): Unregistered script instance: {0} (ID: {1})", ScriptInstanceName, ScriptID.ToString());
	}
}


