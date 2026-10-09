// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "TrainingSettings/StableBaselines/SB3TrainingSettings.h"

void FSB3TrainingSettings::GenerateTrainingArgs(FScriptArgBuilder& ArgBuilder) const
{
	ArgBuilder.AddPositionalArgument("sb3");
	ArgBuilder.AddPositionalArgument("train");

	switch (this->Algorithm)
	{
		case (ESB3TrainingAlgorithm::SAC):
			ArgBuilder.AddPositionalArgument(TEXT("sac"));
			this->SACSettings.GenerateTrainingArgs(ArgBuilder);
			break;
		default:
			ArgBuilder.AddPositionalArgument(TEXT("ppo"));
			this->PPOSettings.GenerateTrainingArgs(ArgBuilder);
			break;
	}
	ArgBuilder.AddPositionalArgument(TEXT("editor"));

	this->CheckpointSettings.GenerateTrainingArgs(ArgBuilder);
	this->LoggingSettings.GenerateTrainingArgs(ArgBuilder);
	this->ResumeSettings.GenerateTrainingArgs(ArgBuilder);
	this->NetworkArchitectureSettings.GenerateTrainingArgs(ArgBuilder);
	// Note that if the NetworkArch Args go right before the Algorithm, the Algorithm gets eaten by the Variable length argument defining network arch.

	ArgBuilder.AddIntArg(TEXT("timesteps"), Timesteps);
	ArgBuilder.AddFlag(TEXT("pbar"), bDisplayProgressBar);
	ArgBuilder.AddFlag(TEXT("disable-eval"), bDisableEval);
}

FSB3TrainingSettings::~FSB3TrainingSettings()
{
}