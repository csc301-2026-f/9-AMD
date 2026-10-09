// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "TrainingSettings/StableBaselines/SB3ResumeSettings.h"

void FSB3ResumeSettings::GenerateTrainingArgs( FScriptArgBuilder& ArgBuilder) const
{
	ArgBuilder.AddConditionalStringArg(TEXT("load-replay-buffer"), this->ReplayBufferPath.FilePath, bLoadReplayBuffer);
	ArgBuilder.AddConditionalStringArg(TEXT("load-vecnormalize"), this->VecNormalizePath.FilePath, bLoadVecNormalize);
	ArgBuilder.AddConditionalStringArg(TEXT("resume-from"), this->ModelPath.FilePath, bLoadModel);
	ArgBuilder.AddFlag(TEXT("reset-timestep"), bLoadModel && bResetTimestep);
}

FSB3ResumeSettings::~FSB3ResumeSettings()
{
}
