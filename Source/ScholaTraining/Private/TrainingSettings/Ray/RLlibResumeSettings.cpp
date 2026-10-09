// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "TrainingSettings/Ray/RLlibResumeSettings.h"

void FRLlibResumeSettings::GenerateTrainingArgs(FScriptArgBuilder& ArgBuilder) const
{
	ArgBuilder.AddConditionalStringArg(TEXT("resume-from"), this->ModelPath.FilePath, bLoadModel);
}

FRLlibResumeSettings::~FRLlibResumeSettings()
{
}