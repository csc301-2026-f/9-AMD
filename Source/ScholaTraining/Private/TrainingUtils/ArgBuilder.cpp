// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "TrainingUtils/ArgBuilder.h"


FScriptArgBuilder& FScriptArgBuilder::AddStringArg(FString ArgName, FString ArgValue)
{
	this->Args.Add(TEXT(" --") + ArgName + TEXT(" ") + ArgValue);
	return *this;
};

FScriptArgBuilder& FScriptArgBuilder::AddFloatArg(FString ArgName, float ArgValue)
{
	return AddStringArg(ArgName, TEXT("\"") + FString::SanitizeFloat(ArgValue) + TEXT("\""));
};

FScriptArgBuilder& FScriptArgBuilder::AddIntArg(FString ArgName, int ArgValue)
{
	return AddStringArg(ArgName, TEXT("\"") + FString::FromInt(ArgValue) + TEXT("\""));
};

FScriptArgBuilder& FScriptArgBuilder::AddFlag(FString FlagName, bool bCondition)
{
	if (bCondition)
	{
		this->Args.Add(TEXT(" --") + FlagName);
	}
	return *this;
};

FScriptArgBuilder& FScriptArgBuilder::AddConditionalStringArg(FString ArgName, FString ArgValue, bool bCondition)
{
	if (bCondition)
	{
		AddStringArg(ArgName, ArgValue);
	}
	return *this;
}
FScriptArgBuilder& FScriptArgBuilder::AddIntArrayArg(FString ArgName, const TArray<int>& ArgValue)
{
	FString ArgString;
	for (const int& Value : ArgValue)
	{
		ArgString += " ";
		ArgString += TEXT("\"") + FString::FromInt(Value) + TEXT("\"");
	}
	return AddStringArg(ArgName, ArgString);
}

FScriptArgBuilder& FScriptArgBuilder::AddPositionalArgument(FString Arg){
	this->Args.Add(TEXT(" ") + Arg);
	return *this;
};

FString FScriptArgBuilder::Build()
{
	FString Output;
	
	for (auto& Arg : this->Args)
	{
		Output += Arg;
	}
	return Output;
};