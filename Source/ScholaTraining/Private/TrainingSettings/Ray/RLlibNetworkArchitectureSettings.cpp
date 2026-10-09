// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "TrainingSettings/Ray/RLlibNetworkArchitectureSettings.h"

void FRLlibNetworkArchSettings::GenerateTrainingArgs(FScriptArgBuilder& ArgBuilder) const
{
	FString ActivationString;
	switch (ActivationFunction)
	{
		case (ERLlibActivationFunctionEnum::TanH):
			ActivationString = TEXT("TanH");
			break;

		case (ERLlibActivationFunctionEnum::ReLU):
			ActivationString = TEXT("ReLU");
			break;

		default:
			ActivationString = TEXT("Sigmoid");
			break;
	}
	ArgBuilder.AddStringArg(TEXT("activation"),ActivationString);
	ArgBuilder.AddIntArrayArg(TEXT("fcnet-hiddens"), this->FCNetHiddens);
	
	if (this->bUseLSTM)
	{
		ArgBuilder.AddFlag(TEXT("use-lstm"));
		ArgBuilder.AddIntArg(TEXT("lstm-cell-size"), this->LSTMCellSize);
		ArgBuilder.AddIntArg(TEXT("max-seq-len"), this->MaxSeqLen);
	}
}

FRLlibNetworkArchSettings::~FRLlibNetworkArchSettings()
{
}