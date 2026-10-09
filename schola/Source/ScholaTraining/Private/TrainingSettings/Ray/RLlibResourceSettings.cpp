// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "TrainingSettings/Ray/RLlibResourceSettings.h"


void FRLlibResourceSettings::GenerateTrainingArgs(FScriptArgBuilder& ArgBuilder) const
{
	if(this->bUseCustomNumberOfCPUs)
	{
		ArgBuilder.AddIntArg(TEXT("num-cpus"),this->NumCPUs);
	}
	ArgBuilder.AddIntArg(TEXT("num-gpus"),this->NumGPUs);
	ArgBuilder.AddIntArg(TEXT("num-cpus-for-main-process"),this->NumCPUsForMainProcess);

	ArgBuilder.AddIntArg(TEXT("num-learners"),this->NumLearners);
	ArgBuilder.AddIntArg(TEXT("num-gpus-per-learner"),this->NumGPUsPerLearner);
	ArgBuilder.AddIntArg(TEXT("num-cpus-per-learner"),this->NumCPUsPerLearner);
}

FRLlibResourceSettings::~FRLlibResourceSettings()
{
}
