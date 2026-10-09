// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "TrainingSettings/Ray/Algorithms/RLlibIMPALASettings.h"

void FRLlibIMPALASettings::GenerateTrainingArgs(FScriptArgBuilder& ArgBuilder) const
{
    ArgBuilder.AddFlag(TEXT("no-vtrace"), !this->bVTrace);
    ArgBuilder.AddFloatArg(TEXT("vtrace-clip-rho-threshold"), this->VTraceClipRhoThreshold);
    ArgBuilder.AddFloatArg(TEXT("vtrace-clip-pg-rho-threshold"), this->VTraceClipPGRhoThreshold);
}

FRLlibIMPALASettings::~FRLlibIMPALASettings()
{
}