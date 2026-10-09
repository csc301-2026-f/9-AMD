// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"

#include "LogScholaTraining.h"
#include "TrainingUtils/ArgBuilder.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTestArgBuilder, "Schola.GymConnectors.Utils.ArgBuilder Test", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FTestArgBuilder::RunTest(const FString& Parameters)
{
	FScriptArgBuilder ArgBuilder;

	ArgBuilder.AddStringArg(TEXT("config"), TEXT("test_config"));
	ArgBuilder.AddIntArg(TEXT("port"), 8000);
	ArgBuilder.AddFloatArg(TEXT("learning-rate"), 0.0003f);
	ArgBuilder.AddFlag(TEXT("enable-checkpoints"));
	ArgBuilder.AddPositionalArgument(TEXT("ppo"));
    
	FString GeneratedArgs = ArgBuilder.Build();

	TestTrue(TEXT("Generated args should contain --config test_config"),
		GeneratedArgs.Contains(TEXT("--config test_config")));
	TestTrue(TEXT("Generated args should contain --port \"8000\""),
		GeneratedArgs.Contains(TEXT("--port \"8000\"")));
	TestTrue(TEXT("Generated args should contain --learning-rate \"0.0003\""),
		GeneratedArgs.Contains(TEXT("--learning-rate \"0.0003\"")));
	TestTrue(TEXT("Generated args should contain --enable-checkpoints flag"),
		GeneratedArgs.Contains(TEXT("--enable-checkpoints")));
	TestTrue(TEXT("Generated args should contain positional argument ppo"),
		GeneratedArgs.Contains(TEXT(" ppo")));

	UE_LOGFMT(LogScholaTraining, Display, "ArgBuilder Test: Generated Args: {0}", GeneratedArgs);

	return true;
}
