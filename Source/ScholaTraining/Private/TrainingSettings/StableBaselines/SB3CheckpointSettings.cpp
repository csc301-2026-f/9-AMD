// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "TrainingSettings/StableBaselines/SB3CheckpointSettings.h"

void FSB3CheckpointSettings::GenerateTrainingArgs(FScriptArgBuilder& ArgBuilder) const
{
	ArgBuilder.AddFlag(TEXT("save-final-policy"), this->bSaveFinalModel);
	if (this->bSaveFinalModel)
	{
		ArgBuilder.AddFlag(TEXT("export-onnx"), this->bExportFinalModelToOnnx);
	}
	
	if (this->bSaveCheckpoints)
	{
		ArgBuilder.AddFlag(TEXT("enable-checkpoints"));
		ArgBuilder.AddIntArg(TEXT("save-freq"), this->SaveFreq);
		ArgBuilder.AddFlag(TEXT("save-replay-buffer"),this->bSaveReplayBuffer);
	}

	// Shared options between the two
	if (this->bSaveFinalModel || this->bSaveCheckpoints)
	{
		ArgBuilder.AddStringArg(TEXT("checkpoint-dir"), this->CheckpointDir.Path);
		ArgBuilder.AddFlag(TEXT("save-vecnormalize"), this->bSaveVecNormalize);
		ArgBuilder.AddStringArg(TEXT("name-prefix-override"), this->NamePrefix);
	}
}

FSB3CheckpointSettings::~FSB3CheckpointSettings()
{
    
}