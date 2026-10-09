// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "TrainingSettings/Ray/RLlibTrainingSettings.h"

void FRLlibTrainingSettings::GenerateTrainingArgs(FScriptArgBuilder& ArgBuilder) const
{
	ArgBuilder.AddPositionalArgument(TEXT("rllib"));
	ArgBuilder.AddPositionalArgument(TEXT("train"));

	switch (this->Algorithm)
	{
		case (ERLlibTrainingAlgorithm::APPO):
			ArgBuilder.AddPositionalArgument(TEXT("appo"));
			this->APPOSettings.GenerateTrainingArgs(ArgBuilder);
			break;
		case (ERLlibTrainingAlgorithm::IMPALA):
			ArgBuilder.AddPositionalArgument(TEXT("impala"));
			this->IMPALASettings.GenerateTrainingArgs(ArgBuilder);
			break;
		case (ERLlibTrainingAlgorithm::SAC):
			ArgBuilder.AddPositionalArgument(TEXT("sac"));
			this->SACSettings.GenerateTrainingArgs(ArgBuilder);
			break;
		default:
			ArgBuilder.AddPositionalArgument(TEXT("ppo"));
			this->PPOSettings.GenerateTrainingArgs(ArgBuilder);
			break;
	}

	ArgBuilder.AddPositionalArgument(TEXT("editor"));

	ArgBuilder.AddIntArg(TEXT("timesteps"), this->Timesteps);
	ArgBuilder.AddFloatArg(TEXT("learning-rate"), this->LearningRate);
	ArgBuilder.AddIntArg(TEXT("minibatch-size"), this->MinibatchSize);
	ArgBuilder.AddIntArg(TEXT("train-batch-size-per-learner"), this->TrainBatchSizePerLearner);
	ArgBuilder.AddIntArg(TEXT("num-sgd-iter"), this->NumSGDIter);
	ArgBuilder.AddFloatArg(TEXT("gamma"), this->Gamma);
	
	this->CheckpointSettings.GenerateTrainingArgs(ArgBuilder);
	this->LoggingSettings.GenerateTrainingArgs(ArgBuilder);
	this->ResumeSettings.GenerateTrainingArgs(ArgBuilder);
	this->NetworkArchitectureSettings.GenerateTrainingArgs(ArgBuilder);
	this->ResourceSettings.GenerateTrainingArgs(ArgBuilder);
}

FRLlibTrainingSettings::~FRLlibTrainingSettings()
{

}