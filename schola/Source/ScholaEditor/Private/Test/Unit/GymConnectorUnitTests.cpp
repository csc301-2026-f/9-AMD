// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "Tests/AutomationEditorCommon.h"
#include "Tests/AutomationCommon.h"
#include "CoreGlobals.h"
#include "Editor.h"
#include "Engine/World.h"
#include "GymConnectors/ManualGymConnector.h"
#include "BasicTestEnvironment.h"
#include "GymConnectors/AbstractGymConnector.h"
#include "Points/DiscretePoint.h"
#include "Points/BoxPoint.h"


// Helper functions for the tests

static UManualGymConnector* SpawnConnector(UWorld* World, FAutomationTestBase* Test)
{
	if (!World)
	{
		return nullptr;
	}

	UManualGymConnector* Connector = NewObject<UManualGymConnector>(World->GetWorldSettings());
	TArray<TScriptInterface<IBaseScholaEnvironment>> Envs;
	Connector->UAbstractGymConnector::CollectEnvironments(Envs);
	Test->AddInfo(FString::Printf(TEXT("Collected %d environment(s)"), Envs.Num()));
	Connector->UAbstractGymConnector::Init(Envs);
	
	return Connector;
}

static ABasicTestMultiAgentEnvironment* SpawnMultiAgentEnv(UWorld* World, FAutomationTestBase* Test)
{
	if (!World)
	{
		return nullptr;
	}

	ABasicTestMultiAgentEnvironment* Env = World->SpawnActor<ABasicTestMultiAgentEnvironment>();
	if (!Env)
	{
		Test->AddError(TEXT("Failed to spawn ABasicMultiAgentTestEnvironment"));
	}
	else
	{
		FVector Loc = Env->GetActorLocation();
		Test->AddInfo(FString::Printf(TEXT("Spawned ABasicMultiAgentTestEnvironment at X=%f Y=%f Z=%f"), Loc.X, Loc.Y, Loc.Z));
	}
	
	return Env;
}

static ABasicTestSingleAgentEnvironment* SpawnSingleAgentEnv(UWorld* World, FAutomationTestBase* Test)
{
	if (!World)
	{
		return nullptr;
	}

	ABasicTestSingleAgentEnvironment* Env = World->SpawnActor<ABasicTestSingleAgentEnvironment>();
	if (!Env)
	{
		Test->AddError(TEXT("Failed to spawn ABasicSingleAgentTestEnvironment"));
	}
	else
	{
		FVector Loc = Env->GetActorLocation();
		Test->AddInfo(FString::Printf(TEXT("Spawned ABasicSingleAgentTestEnvironment at X=%f Y=%f Z=%f"), Loc.X, Loc.Y, Loc.Z));
	}
	
	return Env;
}

static void StepConnector(UManualGymConnector* Connector, ABasicTestEnvironment* Env, int StepNumber, FAutomationTestBase* Test)
{
	if (!Connector || !Env)
	{
		return;
	}

	float BeforeX = Env->GetActorLocation().X;
	float BeforeLogicalX = Env->GetLogicalPositionX();
	
	Test->AddInfo(FString::Printf(TEXT("Step %d BEGIN EnvX=%f LogicalX=%f"), StepNumber, BeforeX, BeforeLogicalX));

	int32 ActionValue = (StepNumber % 2 == 1) ? 1 : 0;
	TMap<FString, TInstancedStruct<FPoint>> Actions;
	Actions.Emplace(Env->AgentName, TInstancedStruct<FPoint>::Make<FDiscretePoint>(ActionValue));
	
	Test->AddInfo(FString::Printf(TEXT("Submitting action for agent '%s' with value=%d"), *Env->AgentName, ActionValue));	

	FInitialState InitState;
	FTrainingState TrainState;
	Connector->ManualStep({ Actions }, InitState, TrainState);

	float AfterX = Env->GetActorLocation().X;
	float AfterLogicalX = Env->GetLogicalPositionX();
	
	Test->AddInfo(FString::Printf(TEXT("Step %d END EnvX=%f LogicalX=%f (Delta=%f)"), StepNumber, AfterX, AfterLogicalX, AfterLogicalX - BeforeLogicalX));	
}

static void ResetConnector(UManualGymConnector* Connector)
{
	if (!Connector)
	{
		return;
	}

	FInitialState InitState;
	Connector->ManualReset(TMap<int32,int32>(), TMap<int,TMap<FString,FString>>(), InitState);
}

static bool VerifyEnvPosition(ABasicTestEnvironment* Env, FAutomationTestBase* Test)
{
	if (!Env)
	{
		Test->AddError(TEXT("Environment missing in verification"));
		return false;
	}
	
	float X = Env->GetActorLocation().X;
	float LogicalX = Env->GetLogicalPositionX();
	
	Test->AddInfo(FString::Printf(TEXT("Env X after steps: World=%f Logical=%f"), X, LogicalX));

	bool bSuccess = true;
	
	if (LogicalX <= 0.f)
	{
		if (Test)
		{
			Test->AddError(TEXT("Environment logical X did not move positive. Check action generation (expected > 0)."));
		}
		bSuccess = false;
	}
	if (FMath::Abs(LogicalX - X) > KINDA_SMALL_NUMBER)
	{
		if (Test) 
		{ 
			Test->AddError(FString::Printf(TEXT("World X (%f) != Logical X (%f)"), X, LogicalX));
		}
		bSuccess = false;
	}
	
	return bSuccess;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSimpleMultiAgentGymConnectorTest, "Schola.GymConnectors.Basic.Multi Agent Env Interface", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FSimpleMultiAgentGymConnectorTest::RunTest(const FString& Parameters)
{
	const int32 TargetSteps = 3;

	AddInfo(FString::Printf(TEXT("Starting test with %d target steps"), TargetSteps));

	// Create test world using FTestWorldWrapper
	FTestWorldWrapper WorldWrapper;
	if (!WorldWrapper.CreateTestWorld(EWorldType::Game))
	{
		AddError(TEXT("Failed to create test world"));
		return false;
	}

	UWorld* World = WorldWrapper.GetTestWorld();
	if (!World)
	{
		AddError(TEXT("Failed to get test world"));
		WorldWrapper.DestroyTestWorld(true);
		return false;
	}

	// Begin play in the test world
	if (!WorldWrapper.BeginPlayInTestWorld())
	{
		AddError(TEXT("Failed to begin play in test world"));
		WorldWrapper.DestroyTestWorld(true);
		return false;
	}
	
	// Tick the world to allow initialization
	for (int32 i = 0; i < 5; ++i)
	{
		WorldWrapper.TickTestWorld(0.016f); // ~60 FPS
	}

	// Spawn environment
	ABasicTestMultiAgentEnvironment* Env = SpawnMultiAgentEnv(World, this);
	if (!Env)
	{
		AddError(TEXT("Failed to spawn multi-agent environment"));
		WorldWrapper.EndPlayInTestWorld();
		WorldWrapper.DestroyTestWorld(true);
		return false;
	}

	// Tick to allow environment to initialize
	WorldWrapper.TickTestWorld(0.016f);

	// Spawn connector
	UManualGymConnector* Connector = SpawnConnector(World, this);
	if (!Connector)
	{
		AddError(TEXT("Failed to spawn connector"));
		WorldWrapper.EndPlayInTestWorld();
		WorldWrapper.DestroyTestWorld(true);
		return false;
	}

	// Reset connector to initialize
	ResetConnector(Connector);
	WorldWrapper.TickTestWorld(0.016f);

	// Step connector multiple times
	for (int32 StepNum = 0; StepNum < TargetSteps; ++StepNum)
	{
		StepConnector(Connector, Env, StepNum, this);
		WorldWrapper.TickTestWorld(0.016f);
	}
	AddInfo(FString::Printf(TEXT(" Completed all %d steps"), TargetSteps));

	// Verify environment position updated
	bool bVerified = VerifyEnvPosition(Env, this);

	// Clean up test world
	WorldWrapper.EndPlayInTestWorld();
	WorldWrapper.DestroyTestWorld(true);

	return bVerified;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSimpleSingleAgentGymConnectorTest, "Schola.GymConnectors.Basic.Single Agent Env Interface", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FSimpleSingleAgentGymConnectorTest::RunTest(const FString& Parameters)
{
	const int32 TargetSteps = 3;

	AddInfo(FString::Printf(TEXT("Starting test with %d target steps"), TargetSteps));

	// Create test world using FTestWorldWrapper
	FTestWorldWrapper WorldWrapper;
	if (!WorldWrapper.CreateTestWorld(EWorldType::Game))
	{
		AddError(TEXT("Failed to create test world"));
		return false;
	}

	UWorld* World = WorldWrapper.GetTestWorld();
	if (!World)
	{
		AddError(TEXT("Failed to get test world"));
		WorldWrapper.DestroyTestWorld(true);
		return false;
	}

	// Begin play in the test world
	if (!WorldWrapper.BeginPlayInTestWorld())
	{
		AddError(TEXT("Failed to begin play in test world"));
		WorldWrapper.DestroyTestWorld(true);
		return false;
	}
	
	// Tick the world to allow initialization
	for (int32 i = 0; i < 5; ++i)
	{
		WorldWrapper.TickTestWorld(0.016f); // ~60 FPS
	}

	// Spawn environment
	ABasicTestSingleAgentEnvironment* Env = SpawnSingleAgentEnv(World, this);
	if (!Env)
	{
		AddError(TEXT("Failed to spawn single-agent environment"));
		WorldWrapper.EndPlayInTestWorld();
		WorldWrapper.DestroyTestWorld(true);
		return false;
	}

	AddInfo(FString::Printf(TEXT("Single-agent environment AgentName='%s'"), *Env->AgentName));

	// Tick to allow environment to initialize
	WorldWrapper.TickTestWorld(0.016f);

	// Spawn connector
	UManualGymConnector* Connector = SpawnConnector(World, this);
	if (!Connector)
	{
		AddError(TEXT("Failed to spawn connector"));
		WorldWrapper.EndPlayInTestWorld();
		WorldWrapper.DestroyTestWorld(true);
		return false;
	}

	// Reset connector to initialize
	ResetConnector(Connector);
	WorldWrapper.TickTestWorld(0.016f);

	// Step connector multiple times
	for (int32 StepNum = 0; StepNum < TargetSteps; ++StepNum)
	{
		StepConnector(Connector, Env, StepNum, this);
		WorldWrapper.TickTestWorld(0.016f);
	}

	AddInfo(FString::Printf(TEXT("Completed all %d steps"), TargetSteps));

	// Verify environment position updated
	bool bVerified = VerifyEnvPosition(Env, this);

	// Clean up test world
	WorldWrapper.EndPlayInTestWorld();
	WorldWrapper.DestroyTestWorld(true);

	return bVerified;
}