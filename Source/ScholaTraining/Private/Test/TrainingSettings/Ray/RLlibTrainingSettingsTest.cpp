// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"

#include "LogScholaTraining.h"
#include "TrainingSettings/Ray/RLlibTrainingSettings.h"
#include "TrainingUtils/ArgBuilder.h"


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRLlibTrainingSettingsTest, "Schola.GymConnectors.Settings.Ray.RLlibTrainingSettings Test", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FRLlibTrainingSettingsTest::RunTest(const FString& Parameters)
{
	FRLlibTrainingSettings RLlibSettings;

	RLlibSettings.Timesteps = 5000;
	RLlibSettings.LearningRate = 0.001f;
	RLlibSettings.MinibatchSize = 256;
	RLlibSettings.Gamma = 0.95f;
	RLlibSettings.Algorithm = ERLlibTrainingAlgorithm::PPO;

	FScriptArgBuilder ArgBuilder;
	RLlibSettings.GenerateTrainingArgs(ArgBuilder);
	FString GeneratedArgs = ArgBuilder.Build();

	TestTrue(TEXT("RLlib args should contain --timesteps \"5000\""),
		GeneratedArgs.Contains(TEXT("--timesteps \"5000\"")));
	TestTrue(TEXT("RLlib args should contain --learning-rate \"0.001\""),
		GeneratedArgs.Contains(TEXT("--learning-rate \"0.001\"")));
	TestTrue(TEXT("RLlib args should contain --minibatch-size \"256\""),
		GeneratedArgs.Contains(TEXT("--minibatch-size \"256\"")));
	TestTrue(TEXT("RLlib args should contain --gamma \"0.95\""),
		GeneratedArgs.Contains(TEXT("--gamma \"0.95\"")));
	TestTrue(TEXT("RLlib args should contain ppo algorithm subcommand (cyclopts names are lowercase)"),
		GeneratedArgs.Contains(TEXT(" ppo ")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPythonGroundTruthRLlibTBTest, "Schola.GymConnectors.PythonGroundTruth RLlib with Tensorboard Test", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FPythonGroundTruthRLlibTBTest::RunTest(const FString& Parameters)
{
	FRLlibTrainingSettings RLlibSettings;
	RLlibSettings.Timesteps = 3000;
	RLlibSettings.LearningRate = 0.0003f;
	RLlibSettings.LoggingSettings.EnvLoggingVerbosity = 0;
	RLlibSettings.LoggingSettings.TrainerLoggingVerbosity = 1;

	FScriptArgBuilder ArgBuilder;
	RLlibSettings.GenerateTrainingArgs(ArgBuilder);
	FString GeneratedArgs = ArgBuilder.Build();

	// Verify key arguments that Python expects
	TestTrue(TEXT("Should contain --schola-verbosity for Python compatibility"),
		GeneratedArgs.Contains(TEXT("--schola-verbosity \"0\"")));
	TestTrue(TEXT("Should contain --rllib-verbosity for Python compatibility"),
		GeneratedArgs.Contains(TEXT("--rllib-verbosity \"1\"")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPythonGroundTruthNetworkArchTest, "Schola.GymConnectors.PythonGroundTruth Network Architecture Test", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FPythonGroundTruthNetworkArchTest::RunTest(const FString& Parameters)
{
	FRLlibTrainingSettings RLlibSettings;
	RLlibSettings.NetworkArchitectureSettings.FCNetHiddens = { 256, 256 };

	FScriptArgBuilder ArgBuilder;
	RLlibSettings.GenerateTrainingArgs(ArgBuilder);
	FString GeneratedArgs = ArgBuilder.Build();

	// Verify network architecture arguments
	TestTrue(TEXT("Should contain --fcnet-hiddens for network architecture"),
		GeneratedArgs.Contains(TEXT("--fcnet-hiddens")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPythonGroundTruthLSTMSettingsTest, "Schola.GymConnectors.PythonGroundTruth RLlib LSTM Settings Test", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FPythonGroundTruthLSTMSettingsTest::RunTest(const FString& Parameters)
{
	FRLlibTrainingSettings RLlibSettings;
	RLlibSettings.NetworkArchitectureSettings.bUseLSTM = true;
	RLlibSettings.NetworkArchitectureSettings.LSTMCellSize = 128;
	RLlibSettings.NetworkArchitectureSettings.MaxSeqLen = 10;

	FScriptArgBuilder ArgBuilder;
	RLlibSettings.GenerateTrainingArgs(ArgBuilder);
	FString GeneratedArgs = ArgBuilder.Build();

	TestTrue(TEXT("Should contain --use-lstm when LSTM is enabled"),
		GeneratedArgs.Contains(TEXT("--use-lstm")));
	TestTrue(TEXT("Should contain --lstm-cell-size \"128\" when LSTM is used"),
		GeneratedArgs.Contains(TEXT("--lstm-cell-size \"128\"")));
	TestTrue(TEXT("Should contain --max-seq-len \"10\" when LSTM is used"),
		GeneratedArgs.Contains(TEXT("--max-seq-len \"10\"")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRLlibSACSettingsTest, "Schola.GymConnectors.Settings.Ray.RLlibSACSettings Test", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FRLlibSACSettingsTest::RunTest(const FString& Parameters)
{
	FRLlibTrainingSettings RLlibSettings;
	RLlibSettings.Algorithm = ERLlibTrainingAlgorithm::SAC;
	RLlibSettings.SACSettings.Tau = 0.005f;
	RLlibSettings.SACSettings.TargetEntropy = TEXT("auto");
	RLlibSettings.SACSettings.InitialAlpha = 1.0f;
	RLlibSettings.SACSettings.NStep = 1;
	RLlibSettings.SACSettings.bTwinQ = true;

	FScriptArgBuilder ArgBuilder;
	RLlibSettings.GenerateTrainingArgs(ArgBuilder);
	FString GeneratedArgs = ArgBuilder.Build();

	TestTrue(TEXT("SAC args should contain sac algorithm subcommand (cyclopts names are lowercase)"),
		GeneratedArgs.Contains(TEXT(" sac ")));
	TestTrue(TEXT("SAC args should contain --tau"),
		GeneratedArgs.Contains(TEXT("--tau")));
	TestTrue(TEXT("SAC args should contain --target-entropy"),
		GeneratedArgs.Contains(TEXT("--target-entropy")));
	TestTrue(TEXT("SAC args should contain --initial-alpha"),
		GeneratedArgs.Contains(TEXT("--initial-alpha")));
	TestTrue(TEXT("SAC args should contain --n-step"),
		GeneratedArgs.Contains(TEXT("--n-step")));
	TestTrue(TEXT("SAC args should not contain --no-twin-q when bTwinQ is true"),
		!GeneratedArgs.Contains(TEXT("--no-twin-q")));

	// Test twin_q false adds --no-twin-q
	RLlibSettings.SACSettings.bTwinQ = false;
	FScriptArgBuilder ArgBuilder2;
	RLlibSettings.GenerateTrainingArgs(ArgBuilder2);
	FString GeneratedArgs2 = ArgBuilder2.Build();
	TestTrue(TEXT("SAC args should contain --no-twin-q when bTwinQ is false"),
		GeneratedArgs2.Contains(TEXT("--no-twin-q")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FConditionalFlagsTest, "Schola.GymConnectors.ConditionalFlags Test", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FConditionalFlagsTest::RunTest(const FString& Parameters)
{
	FRLlibTrainingSettings RLlibSettings;

	RLlibSettings.CheckpointSettings.bEnableCheckpoints = true;
	RLlibSettings.CheckpointSettings.SaveFreq = 2000;
	RLlibSettings.CheckpointSettings.bSaveFinalModel = true;
	RLlibSettings.CheckpointSettings.bExportToONNX = true;

	FScriptArgBuilder ArgBuilder;
	RLlibSettings.GenerateTrainingArgs(ArgBuilder);
	FString GeneratedArgs = ArgBuilder.Build();

	TestTrue(TEXT("Should contain --enable-checkpoints when bEnableCheckpoints is true"),
		GeneratedArgs.Contains(TEXT("--enable-checkpoints")));
	TestTrue(TEXT("Should contain --save-freq \"2000\""),
		GeneratedArgs.Contains(TEXT("--save-freq \"2000\"")));
	TestTrue(TEXT("Should contain --save-final-policy when bSaveFinalModel is true"),
		GeneratedArgs.Contains(TEXT("--save-final-policy")));
	TestTrue(TEXT("Should contain --export-onnx when bExportToONNX is true"),
		GeneratedArgs.Contains(TEXT("--export-onnx")));

	UE_LOGFMT(LogScholaTraining, Display, "Conditional Flags Test: Generated Args: {0}", GeneratedArgs);

	return true;
}