// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "TrainingSettings/Ray/RLlibLoggingSettings.h"

void FRLlibLoggingSettings::GenerateTrainingArgs(FScriptArgBuilder& ArgBuilder) const
{
	ArgBuilder.AddIntArg(TEXT("schola-verbosity"), EnvLoggingVerbosity);
	ArgBuilder.AddIntArg(TEXT("rllib-verbosity"), TrainerLoggingVerbosity);
}

FRLlibLoggingSettings::~FRLlibLoggingSettings()
{
    
}