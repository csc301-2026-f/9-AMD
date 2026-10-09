// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "TrainingSettings/StableBaselines/Algorithms/SB3PPOSettings.h"

void FSB3PPOSettings::GenerateTrainingArgs( FScriptArgBuilder& ArgBuilder) const
{
	ArgBuilder.AddFloatArg(TEXT("learning-rate"),this->LearningRate);
	ArgBuilder.AddIntArg(TEXT("n-steps"),this->NSteps);
	ArgBuilder.AddIntArg(TEXT("batch-size"),this->BatchSize);
	ArgBuilder.AddIntArg(TEXT("n-epochs"), this->NEpochs);
	
    ArgBuilder.AddFloatArg(TEXT("gamma"), this->Gamma);
    ArgBuilder.AddFloatArg(TEXT("gae-lambda"), this->GAELambda);
	ArgBuilder.AddFloatArg(TEXT("clip-range"), this->ClipRange);

	ArgBuilder.AddFlag("normalize-advantage",this->NormalizeAdvantage);
    ArgBuilder.AddFloatArg(TEXT("ent-coef"), this->EntCoef);
    ArgBuilder.AddFloatArg(TEXT("vf-coef"), this->VFCoef);
    ArgBuilder.AddFloatArg(TEXT("max-grad-norm"), this->MaxGradNorm);

	ArgBuilder.AddFlag(TEXT("use-sde"),this->UseSDE);
	ArgBuilder.AddIntArg(TEXT("sde-sample-freq"),this->SDESampleFreq);
	
}

FSB3PPOSettings::~FSB3PPOSettings()
{
}