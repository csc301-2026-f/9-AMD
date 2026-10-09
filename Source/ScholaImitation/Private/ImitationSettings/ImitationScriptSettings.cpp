// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "ImitationSettings/ImitationScriptSettings.h"
#include "Misc/Paths.h"
#include "HAL/PlatformProcess.h"
#include "Interfaces/IPluginManager.h"
#include "LogScholaImitation.h"

void FImitationScriptSettings::GetArgs(FScriptArgBuilder& InArgBuilder) const
{
	this->MinariSettings.GenerateImitationArgs(InArgBuilder);
}


FString FImitationScriptSettings::GetScriptPath() const
{
	return FString("-m schola.scripts.launch");		
}

FImitationScriptSettings::~FImitationScriptSettings()
{
}