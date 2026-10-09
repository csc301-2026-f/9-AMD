// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "TrainingSettings/StableBaselines/Algorithms/SB3SACSettings.h"

void FSB3SACSettings::GenerateTrainingArgs( FScriptArgBuilder& ArgBuilder) const
{
	ArgBuilder.AddFloatArg(TEXT("learning-rate"),this->LearningRate);
	ArgBuilder.AddIntArg(TEXT("buffer-size"), this->BufferSize);
	ArgBuilder.AddIntArg(TEXT("learning-starts"), this->LearningStarts);
	ArgBuilder.AddIntArg(TEXT("batch-size"), this->BatchSize);
	ArgBuilder.AddFlag(TEXT("optimize-memory-usage"), this->OptimizeMemoryUsage);

	ArgBuilder.AddFloatArg(TEXT("tau"), this->Tau);
	ArgBuilder.AddFloatArg(TEXT("gamma"), this->Gamma);

	ArgBuilder.AddIntArg(TEXT("train-freq"), this->TrainFreq);
	ArgBuilder.AddIntArg(TEXT("gradient-steps"), this->GradientSteps);
	
	
	FString EntCoefString = (this->LearnEntCoef ? FString("auto_") : FString(""))+ FString::SanitizeFloat(this->InitialEntCoef);
	ArgBuilder.AddStringArg(TEXT("ent-coef"), EntCoefString);

	ArgBuilder.AddIntArg(TEXT("target-update-interval"), this->TargetUpdateInterval);

	ArgBuilder.AddStringArg(TEXT("target-entropy"), this->TargetEntropy);
	
	ArgBuilder.AddFlag(TEXT("use-sde"),this->UseSDE);
	ArgBuilder.AddIntArg(TEXT("sde-sample-freq"),this->SDESampleFreq);
}
FSB3SACSettings::~FSB3SACSettings(){};