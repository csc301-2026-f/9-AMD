// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "TrainingSettings/StableBaselines/SB3LoggingSettings.h"

void FSB3LoggingSettings::GenerateTrainingArgs( FScriptArgBuilder& ArgBuilder) const
{
	ArgBuilder.AddIntArg(TEXT("schola-verbosity"), EnvLoggingVerbosity);
	ArgBuilder.AddIntArg(TEXT("sb3-verbosity"), TrainerLoggingVerbosity);
	
	if (this->bSaveTBLogs)
	{
		ArgBuilder.AddFlag(TEXT("enable-tensorboard"));
		ArgBuilder.AddStringArg(TEXT("log-dir"),this->LogDir.Path);
		ArgBuilder.AddIntArg(TEXT("log-freq"), this->LogFreq);
		ArgBuilder.AddIntArg(TEXT("callback-verbosity"), this->CallbackVerbosity);
	}
}

FSB3LoggingSettings::~FSB3LoggingSettings()
{
}