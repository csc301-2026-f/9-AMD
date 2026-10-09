// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "TrainingSettings/TrainingScriptSettings.h"
#include "Misc/Paths.h"
#include "HAL/PlatformProcess.h"
#include "Interfaces/IPluginManager.h"
#include "LogScholaTraining.h"

inline FString WithQuotes(FString Input)
{
	return FString("\"") + Input + FString("\"");
}

void FTrainingScriptSettings::GetArgs(FScriptArgBuilder& InArgBuilder) const
{
	//Pass in an ArgBuilder to add additional arguments to the script
	switch (ScriptType)
	{
		case (EScriptType::Python):
			switch (PythonScriptType)
			{
				case (EPythonScript::SB3):
					this->SB3Settings.GenerateTrainingArgs(InArgBuilder);
					break;
				case (EPythonScript::RLLIB):
					this->RLlibSettings.GenerateTrainingArgs(InArgBuilder);
					break;
				default:
					this->CustomPythonScriptSettings.GenerateTrainingArgs(InArgBuilder);
					break;
			}
			break;

		default:
			this->CustomScriptSettings.GenerateTrainingArgs(InArgBuilder);
			break;
	}
}

FString FTrainingScriptSettings::GetScriptPath() const
{
	switch (ScriptType)
	{
		case (EScriptType::Python):
			switch (PythonScriptType)
			{
				case (EPythonScript::Custom):
					return WithQuotes(CustomPythonScriptSettings.LaunchScript.FilePath);
				default:
					return FString("-m schola.scripts.launch");
			}

		default:
			return WithQuotes(CustomScriptSettings.LaunchScript.FilePath);
	}
}

FTrainingScriptSettings::~FTrainingScriptSettings()
{
}
