// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "TrainingSettings/Ray/Algorithms/RLlibSACSettings.h"

void FRLlibSACSettings::GenerateTrainingArgs(FScriptArgBuilder& ArgBuilder) const
{
	ArgBuilder.AddFloatArg(TEXT("tau"), this->Tau);
	ArgBuilder.AddStringArg(TEXT("target-entropy"), this->TargetEntropy);
	ArgBuilder.AddFloatArg(TEXT("initial-alpha"), this->InitialAlpha);
	ArgBuilder.AddIntArg(TEXT("n-step"), this->NStep);
	ArgBuilder.AddFlag(TEXT("no-twin-q"), !this->bTwinQ);
}

FRLlibSACSettings::~FRLlibSACSettings()
{
}
