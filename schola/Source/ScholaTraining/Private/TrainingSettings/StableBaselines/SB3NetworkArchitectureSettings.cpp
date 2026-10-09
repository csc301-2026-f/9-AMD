// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "TrainingSettings/StableBaselines/SB3NetworkArchitectureSettings.h"

void FSB3NetworkArchSettings::GenerateTrainingArgs( FScriptArgBuilder& ArgBuilder) const
{
	//This has to be non-lowercase due to parsing issue with cyclopts
	FString ActivationString;
	switch (ActivationFunction)
	{
		case (ESB3ActivationFunctionEnum::TanH):
			ActivationString = TEXT("TanH");
			break;

		case (ESB3ActivationFunctionEnum::ReLU):
			ActivationString = TEXT("ReLU");
			break;

		default:
			ActivationString = TEXT("Sigmoid");
			break;
	}
	ArgBuilder.AddStringArg(TEXT("activation"), ActivationString);
	ArgBuilder.AddIntArrayArg(TEXT("critic-parameters"), this->CriticParameters);
	ArgBuilder.AddIntArrayArg(TEXT("policy-parameters"), this->PolicyParameters);
}


FSB3NetworkArchSettings::~FSB3NetworkArchSettings()
{

}