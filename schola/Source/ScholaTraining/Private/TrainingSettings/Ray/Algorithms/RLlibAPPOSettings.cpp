// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "TrainingSettings/Ray/Algorithms/RLlibAPPOSettings.h"

void FRLlibAPPOSettings::GenerateTrainingArgs(FScriptArgBuilder& ArgBuilder) const
{
    ArgBuilder.AddFlag(TEXT("no-vtrace"), !this->bVTrace);
    ArgBuilder.AddFloatArg(TEXT("vtrace-clip-rho-threshold"), this->VTraceClipRhoThreshold);
    ArgBuilder.AddFloatArg(TEXT("vtrace-clip-pg-rho-threshold"), this->VTraceClipPGRhoThreshold);
    ArgBuilder.AddFloatArg(TEXT("gae-lambda"), this->GAELambda);
    ArgBuilder.AddFloatArg(TEXT("clip-param"), this->ClipParam);
    ArgBuilder.AddFlag(TEXT("no-use-gae"), !this->bUseGAE);
}

FRLlibAPPOSettings::~FRLlibAPPOSettings()
{

}


