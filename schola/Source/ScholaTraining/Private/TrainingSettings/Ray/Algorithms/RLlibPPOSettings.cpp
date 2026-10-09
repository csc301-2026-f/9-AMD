// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "TrainingSettings/Ray/Algorithms/RLlibPPOSettings.h"

void FRLlibPPOSettings::GenerateTrainingArgs(FScriptArgBuilder& ArgBuilder) const
{
    ArgBuilder.AddFloatArg(TEXT("gae-lambda"), this->GAELambda);
    ArgBuilder.AddFloatArg(TEXT("clip-param"), this->ClipParam);
    ArgBuilder.AddFlag(TEXT("no-use-gae"), !this->bUseGAE);
    
}

FRLlibPPOSettings::~FRLlibPPOSettings()
{
    
}