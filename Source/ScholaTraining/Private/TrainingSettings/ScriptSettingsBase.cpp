// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "TrainingSettings/ScriptSettingsBase.h"
#include "Misc/Paths.h"
#include "HAL/PlatformProcess.h"
#include "Interfaces/IPluginManager.h"
#include "LogScholaTraining.h"

FLaunchableScript FScriptSettingsBase::GetLaunchableScript() const
{	
	FString ScriptCommand = this->GetScriptPath();
	switch (ScriptType)
	{
		case (EScriptType::Python):
			switch (EnvType)
			{
				case (EPythonEnvironmentType::Conda):
				{
#if PLATFORM_WINDOWS
					// Using explicit .bat as sometimes just conda doesn't get picked up correctly on Windows
					const FString CondaExec = TEXT("conda.bat"); 
#else
					const FString CondaExec = TEXT("conda");
#endif
					const FString Args = FString::Printf(
						TEXT("run --live-stream -n \"%s\" python %s"),
						*this->CondaEnvName,
						*ScriptCommand);
					return FLaunchableScript(CondaExec, Args);
				}
				case (EPythonEnvironmentType::VEnv):
					//Convert to absolute file path
					if (this->CustomPythonPath.FilePath.StartsWith(TEXT("..")))
					{
						return FLaunchableScript(FPaths::ConvertRelativePathToFull(this->CustomPythonPath.FilePath), ScriptCommand);
					}
					else
					{
						return FLaunchableScript(this->CustomPythonPath.FilePath, ScriptCommand);
					}

				case (EPythonEnvironmentType::SystemPath):
					return FLaunchableScript(FString("python"), ScriptCommand);

				default:
					EnsureScholaIsInstalled();
					return FLaunchableScript(GetBuiltInPythonPath(), ScriptCommand);
			}

		default:
			return FLaunchableScript(ScriptCommand);
	}
}

FString FScriptSettingsBase::GetBuiltInPythonPath() const
{
#if PLATFORM_WINDOWS
	return FPaths::Combine(FPaths::EngineDir(), TEXT("Binaries/ThirdParty/Python3/Win64/python.exe"));
#elif PLATFORM_MAC
	return FPaths::Combine(FPaths::EngineDir(), TEXT("Binaries/ThirdParty/Python3/Mac/bin/python3"));
#elif PLATFORM_LINUX
	return FPaths::Combine(FPaths::EngineDir(), TEXT("Binaries/ThirdParty/Python3/Linux/bin/python3"));
#else
	UE_LOGFMT(LogScholaTraining, Warning, "FScriptSettingsBase::GetBuiltInPythonPath(): Unsupported platform, defaulting to Linux Python path");
	return FPaths::Combine(FPaths::EngineDir(), TEXT("Binaries/ThirdParty/Python3/Linux/bin/python3"));
#endif
}

void FScriptSettingsBase::EnsureScholaIsInstalled() const
{
	FString PythonPath = GetBuiltInPythonPath();
	int32 ReturnCode;
	FString StdOut;
	FString StdErr;
	FPlatformProcess::ExecProcess(*PythonPath, TEXT("-m pip show schola"), &ReturnCode, &StdOut, &StdErr);
	// if not installed, install it
	if (ReturnCode != 0)
	{
		FString Command = FString::Printf(TEXT("-m pip install %s[all]"), *(*IPluginManager::Get().FindPlugin(TEXT("Schola"))->GetBaseDir() + FString("/Resources/python")));
		FPlatformProcess::ExecProcess(*PythonPath, *Command, &ReturnCode, &StdOut, &StdErr);
		if (ReturnCode != 0)
		{
			UE_LOGFMT(LogScholaTraining, Error, "FScriptSettingsBase::EnsureScholaIsInstalled(): Failed to install Schola python package: {0}\n{1}", StdOut, StdErr);
		} else{
			UE_LOGFMT(LogScholaTraining, Log, "FScriptSettingsBase::EnsureScholaIsInstalled(): Installed Schola python package");
		}	
	}
}

FScriptSettingsBase::~FScriptSettingsBase()
{
}

FString FScriptSettingsBase::GetArgs() const
{
	FScriptArgBuilder ArgBuilder;
	GetArgs(ArgBuilder);
	return ArgBuilder.Build();
}


