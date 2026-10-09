// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "ImitationSettings/ImitationLoggingSettings.h"

void FImitationLoggingSettings::GenerateImitationArgs(FScriptArgBuilder& ArgBuilder) const
{
	ArgBuilder.AddIntArg(TEXT("schola-verbosity"), ScholaVerbosity);
}

FImitationLoggingSettings::~FImitationLoggingSettings()
{
}
