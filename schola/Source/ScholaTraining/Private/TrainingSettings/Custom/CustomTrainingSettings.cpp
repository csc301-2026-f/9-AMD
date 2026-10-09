// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "TrainingSettings/Custom/CustomTrainingSettings.h"

void FCustomTrainingSettings::GenerateTrainingArgs( FScriptArgBuilder& ArgBuilder) const
{
	FString Output;

	
	for (auto& Elem : this->Flags)
	{
		ArgBuilder.AddFlag(Elem,true);
	}

	for (auto& Elem : this->Args)
	{
		ArgBuilder.AddStringArg(Elem.Key, Elem.Value);
	}
}

FCustomTrainingSettings::~FCustomTrainingSettings()
{
    
}