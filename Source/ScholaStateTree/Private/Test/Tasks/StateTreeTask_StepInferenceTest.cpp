// Copyright (c) 2026 Advanced Micro Devices, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"

#include "Tasks/StateTreeTask_StepInference.h"
#include "Common/InteractionDefinition.h"

#if WITH_DEV_AUTOMATION_TESTS

// ========== Default Configuration Tests ==========

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FStepInferenceDefaultConfigTest, "Schola.StateTree.Tasks.StepInference.DefaultConfig", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FStepInferenceDefaultConfigTest::RunTest(const FString& Parameters)
{
	UStateTreeTask_StepInference* Task = GetMutableDefault<UStateTreeTask_StepInference>();

	// Policy is not created by default - user must assign a policy with a model
	TestNull(TEXT("Policy should be null by default (user must configure)"), Task->Policy.Get());
	TestFalse(TEXT("bCompleteAfterSingleStep should default to false"), Task->bCompleteAfterSingleStep);

	IAgent* AgentInterface = Cast<IAgent>(Task);
	TestNotNull(TEXT("Task should implement IAgent interface"), AgentInterface);

	return true;
}

// ========== IAgent Interface Tests ==========

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FStepInferenceAgentInterfaceTest, "Schola.StateTree.Tasks.StepInference.AgentInterface", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FStepInferenceAgentInterfaceTest::RunTest(const FString& Parameters)
{
	UClass* TaskClass = UStateTreeTask_StepInference::StaticClass();
	TestTrue(TEXT("Task class should implement IAgent"), TaskClass->ImplementsInterface(UAgent::StaticClass()));

	UStateTreeTask_StepInference* Task = GetMutableDefault<UStateTreeTask_StepInference>();
	FInteractionDefinition		  Defn;
	IAgent::Execute_Define(Task, Defn);
	TestFalse(TEXT("Default Define should not set ObsSpaceDefn"), Defn.ObsSpaceDefn.IsValid());
	TestFalse(TEXT("Default Define should not set ActionSpaceDefn"), Defn.ActionSpaceDefn.IsValid());

	return true;
}

// ========== ResetForEpisode Tests ==========

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FStepInferenceResetForEpisodeTest, "Schola.StateTree.Tasks.StepInference.ResetForEpisode", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FStepInferenceResetForEpisodeTest::RunTest(const FString& Parameters)
{
	UStateTreeTask_StepInference* Task = NewObject<UStateTreeTask_StepInference>();

	// ResetForEpisode should be callable and set context
	FInstancedStruct OutObs;
	Task->ResetForEpisode(nullptr, OutObs);

	// Default implementation calls Observe, which returns empty for base class
	TestFalse(TEXT("Default ResetForEpisode should call Observe (returns invalid for base)"), OutObs.IsValid());

	return true;
}

// ========== Context Actor Tests ==========

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FStepInferenceNoContextActorTest, "Schola.StateTree.Tasks.StepInference.NoContextActor", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FStepInferenceNoContextActorTest::RunTest(const FString& Parameters)
{
	UStateTreeTask_StepInference* Task = NewObject<UStateTreeTask_StepInference>();

	// Without EnterState, GetContextActor should return nullptr
	AActor* Actor = Task->GetContextActor();
	TestNull(TEXT("GetContextActor should return nullptr before EnterState"), Actor);

	return true;
}

// ========== Training Mode Tests ==========

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FStepInferenceTrainingModeDefaultTest, "Schola.StateTree.Tasks.StepInference.TrainingModeDefault", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FStepInferenceTrainingModeDefaultTest::RunTest(const FString& Parameters)
{
	UStateTreeTask_StepInference* Task = NewObject<UStateTreeTask_StepInference>();

	// Without EnterState, should not be in training mode
	TestFalse(TEXT("IsTrainingMode should return false before EnterState"), Task->IsTrainingMode());

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
