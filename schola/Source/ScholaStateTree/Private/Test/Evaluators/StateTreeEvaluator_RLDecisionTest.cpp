// Copyright (c) 2026 Advanced Micro Devices, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"

#include "Evaluators/StateTreeEvaluator_RLDecision.h"
#include "Common/InteractionDefinition.h"

#if WITH_DEV_AUTOMATION_TESTS

// ========== Default Configuration Tests ==========

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRLDecisionInferenceDefaultsTest, "Schola.StateTree.Evaluators.RLDecision.Defaults", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FRLDecisionInferenceDefaultsTest::RunTest(const FString& Parameters)
{
	UStateTreeEvaluator_RLDecision* Evaluator = NewObject<UStateTreeEvaluator_RLDecision>();

	// Output defaults
	TestEqual(TEXT("Default SelectedBranch should be -1"), Evaluator->SelectedBranch, -1);
	TestEqual(TEXT("Default NumBranches should be 0"), Evaluator->NumBranches, 0);
	TestEqual(TEXT("Default Confidence should be 0.0"), Evaluator->Confidence, 0.0f);
	TestEqual(TEXT("Default bLastInferenceSucceeded should be false"), Evaluator->bLastInferenceSucceeded, false);

	// Configuration defaults
	TestEqual(TEXT("Default bReevaluateEveryTick should be true"), Evaluator->bReevaluateEveryTick, true);
	TestEqual(TEXT("Default ReevaluationInterval should be 0.0"), Evaluator->ReevaluationInterval, 0.0f);

	// Policy should be null by default
	TestNull(TEXT("Default Policy should be nullptr"), Evaluator->Policy);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRLDecisionInferenceConfigurableTest, "Schola.StateTree.Evaluators.RLDecision.Configurable", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FRLDecisionInferenceConfigurableTest::RunTest(const FString& Parameters)
{
	UStateTreeEvaluator_RLDecision* Evaluator = NewObject<UStateTreeEvaluator_RLDecision>();

	// Test setting bReevaluateEveryTick
	Evaluator->bReevaluateEveryTick = false;
	TestEqual(TEXT("bReevaluateEveryTick should be configurable"), Evaluator->bReevaluateEveryTick, false);

	// Test setting ReevaluationInterval
	Evaluator->ReevaluationInterval = 0.5f;
	TestEqual(TEXT("ReevaluationInterval should be configurable"), Evaluator->ReevaluationInterval, 0.5f);

	return true;
}

// ========== Output Properties Tests ==========

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRLDecisionInferenceBranchScoresTest, "Schola.StateTree.Evaluators.RLDecision.BranchScores", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FRLDecisionInferenceBranchScoresTest::RunTest(const FString& Parameters)
{
	UStateTreeEvaluator_RLDecision* Evaluator = NewObject<UStateTreeEvaluator_RLDecision>();

	// BranchScores should be empty by default
	TestEqual(TEXT("Default BranchScores should be empty"), Evaluator->BranchScores.Num(), 0);

	// Manually set scores for testing
	const TArray<float> ExpectedScores = {0.2f, 0.5f, 0.3f};
	Evaluator->BranchScores = ExpectedScores;
	TestEqual(TEXT("BranchScores"), Evaluator->BranchScores, ExpectedScores);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRLDecisionInferenceReevaluationIntervalRangeTest, "Schola.StateTree.Evaluators.RLDecision.ReevaluationIntervalRange", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FRLDecisionInferenceReevaluationIntervalRangeTest::RunTest(const FString& Parameters)
{
	UStateTreeEvaluator_RLDecision* Evaluator = NewObject<UStateTreeEvaluator_RLDecision>();

	// Test various interval values
	Evaluator->ReevaluationInterval = 0.0f;
	TestEqual(TEXT("ReevaluationInterval 0 should work (every tick)"), Evaluator->ReevaluationInterval, 0.0f);

	Evaluator->ReevaluationInterval = 0.1f;
	TestEqual(TEXT("ReevaluationInterval 0.1 should work"), Evaluator->ReevaluationInterval, 0.1f);

	Evaluator->ReevaluationInterval = 1.0f;
	TestEqual(TEXT("ReevaluationInterval 1.0 should work"), Evaluator->ReevaluationInterval, 1.0f);

	return true;
}

// ========== Output State Tests ==========

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRLDecisionInferenceOutputStateTest, "Schola.StateTree.Evaluators.RLDecision.OutputState", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FRLDecisionInferenceOutputStateTest::RunTest(const FString& Parameters)
{
	UStateTreeEvaluator_RLDecision* Evaluator = NewObject<UStateTreeEvaluator_RLDecision>();

	// Simulate setting output state (as if after inference)
	Evaluator->SelectedBranch = 2;
	Evaluator->NumBranches = 4;
	Evaluator->Confidence = 0.95f;
	Evaluator->bLastInferenceSucceeded = true;

	TestEqual(TEXT("SelectedBranch should be 2"), Evaluator->SelectedBranch, 2);
	TestEqual(TEXT("NumBranches should be 4"), Evaluator->NumBranches, 4);
	TestEqual(TEXT("Confidence should be 0.95"), Evaluator->Confidence, 0.95f);
	TestEqual(TEXT("bLastInferenceSucceeded should be true"), Evaluator->bLastInferenceSucceeded, true);

	return true;
}

// ========== Context Actor Tests ==========

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRLDecisionInferenceNoContextActorTest, "Schola.StateTree.Evaluators.RLDecision.NoContextActor", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FRLDecisionInferenceNoContextActorTest::RunTest(const FString& Parameters)
{
	UStateTreeEvaluator_RLDecision* Evaluator = NewObject<UStateTreeEvaluator_RLDecision>();

	// Without TreeStart, GetContextActor should return nullptr
	AActor* Actor = Evaluator->GetContextActor();
	TestNull(TEXT("GetContextActor should return nullptr before TreeStart"), Actor);

	return true;
}

// ========== IAgent Interface Tests ==========

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRLDecisionAgentInterfaceTest, "Schola.StateTree.Evaluators.RLDecision.AgentInterface", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FRLDecisionAgentInterfaceTest::RunTest(const FString& Parameters)
{
	UClass* EvaluatorClass = UStateTreeEvaluator_RLDecision::StaticClass();
	TestTrue(TEXT("Evaluator class should implement IAgent"), EvaluatorClass->ImplementsInterface(UAgent::StaticClass()));

	UStateTreeEvaluator_RLDecision* Evaluator = GetMutableDefault<UStateTreeEvaluator_RLDecision>();
	FInteractionDefinition			Defn;
	IAgent::Execute_Define(Evaluator, Defn);
	TestFalse(TEXT("Default Define should not set ObsSpaceDefn"), Defn.ObsSpaceDefn.IsValid());
	TestFalse(TEXT("Default Define should not set ActionSpaceDefn"), Defn.ActionSpaceDefn.IsValid());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRLDecisionOutputPropertiesTest, "Schola.StateTree.Evaluators.RLDecision.OutputProperties", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FRLDecisionOutputPropertiesTest::RunTest(const FString& Parameters)
{
	UClass* EvaluatorClass = UStateTreeEvaluator_RLDecision::StaticClass();

	TestNotNull(TEXT("SelectedBranch property should exist"), EvaluatorClass->FindPropertyByName(TEXT("SelectedBranch")));
	TestNotNull(TEXT("NumBranches property should exist"), EvaluatorClass->FindPropertyByName(TEXT("NumBranches")));
	TestNotNull(TEXT("Confidence property should exist"), EvaluatorClass->FindPropertyByName(TEXT("Confidence")));
	TestNotNull(TEXT("BranchScores property should exist"), EvaluatorClass->FindPropertyByName(TEXT("BranchScores")));
	TestNotNull(TEXT("bLastInferenceSucceeded property should exist"), EvaluatorClass->FindPropertyByName(TEXT("bLastInferenceSucceeded")));

	return true;
}

// ========== ResetForEpisode Tests ==========

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRLDecisionResetForEpisodeTest, "Schola.StateTree.Evaluators.RLDecision.ResetForEpisode", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FRLDecisionResetForEpisodeTest::RunTest(const FString& Parameters)
{
	UStateTreeEvaluator_RLDecision* Evaluator = NewObject<UStateTreeEvaluator_RLDecision>();

	// Set branch to non-default
	Evaluator->SelectedBranch = 5;

	// ResetForEpisode should reset SelectedBranch to -1
	FInstancedStruct OutObs;
	Evaluator->ResetForEpisode(nullptr, OutObs);

	TestEqual(TEXT("ResetForEpisode should reset SelectedBranch to -1"), Evaluator->SelectedBranch, -1);

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
