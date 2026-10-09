// Copyright (c) 2026 Advanced Micro Devices, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"

#include "RLDecisionHelpers.h"
#include "Points/DiscretePoint.h"
#include "Points/DictPoint.h"
#include "Points/BoxPoint.h"
#include "Points/MultiDiscretePoint.h"
#include "Spaces/DiscreteSpace.h"
#include "Spaces/BoxSpace.h"

#if WITH_DEV_AUTOMATION_TESTS

// ========== Extract Branch Index Tests ==========

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBranchExtractionDiscretePointTest, "Schola.StateTree.Helpers.BranchExtraction.DiscretePoint", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FBranchExtractionDiscretePointTest::RunTest(const FString& Parameters)
{
	TInstancedStruct<FPoint> Action = TInstancedStruct<FPoint>::Make<FDiscretePoint>(2);

	int32		  NumBranches = 5;
	TArray<float> BranchScores;
	float		  Confidence = 0.0f;

	int32 Result = RLDecisionHelpers::ExtractBranchIndexFromAction(Action, NumBranches, BranchScores, Confidence);

	TestEqual(TEXT("DiscretePoint should return correct branch index"), Result, 2);
	TestEqual(TEXT("Confidence should be 1.0 for discrete"), Confidence, 1.0f);
	TestEqual(TEXT("BranchScores should have correct size"), BranchScores.Num(), 5);
	TestEqual(TEXT("Score for selected branch should be 1.0"), BranchScores[2], 1.0f);
	TestEqual(TEXT("Score for other branches should be 0.0"), BranchScores[0], 0.0f);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBranchExtractionDictPointTest, "Schola.StateTree.Helpers.BranchExtraction.DictPoint", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FBranchExtractionDictPointTest::RunTest(const FString& Parameters)
{
	FDictPoint DictPoint;
	DictPoint.Points.Add(TEXT("action"), TInstancedStruct<FPoint>::Make<FDiscretePoint>(1));
	TInstancedStruct<FPoint> Action = TInstancedStruct<FPoint>::Make<FDictPoint>(DictPoint);

	int32		  NumBranches = 3;
	TArray<float> BranchScores;
	float		  Confidence = 0.0f;

	int32 Result = RLDecisionHelpers::ExtractBranchIndexFromAction(Action, NumBranches, BranchScores, Confidence);

	TestEqual(TEXT("DictPoint with discrete should return correct branch"), Result, 1);
	TestEqual(TEXT("Confidence should be 1.0"), Confidence, 1.0f);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBranchExtractionBoxPointArgmaxTest, "Schola.StateTree.Helpers.BranchExtraction.BoxPointArgmax", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FBranchExtractionBoxPointArgmaxTest::RunTest(const FString& Parameters)
{
	FBoxPoint BoxPoint;
	BoxPoint.Values = { 0.1f, 0.3f, 0.9f, 0.2f };
	TInstancedStruct<FPoint> Action = TInstancedStruct<FPoint>::Make<FBoxPoint>(BoxPoint);

	int32		  NumBranches = 0;
	TArray<float> BranchScores;
	float		  Confidence = 0.0f;

	int32 Result = RLDecisionHelpers::ExtractBranchIndexFromAction(Action, NumBranches, BranchScores, Confidence);

	TestEqual(TEXT("BoxPoint argmax should return index 2"), Result, 2);
	TestEqual(TEXT("NumBranches should be updated to array size"), NumBranches, 4);
	TestEqual(TEXT("Confidence should be the max value"), Confidence, 0.9f);
	TestEqual(TEXT("BranchScores should match input"), BranchScores[0], 0.1f);
	TestEqual(TEXT("BranchScores should match input"), BranchScores[2], 0.9f);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBranchExtractionInvalidTest, "Schola.StateTree.Helpers.BranchExtraction.Invalid", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FBranchExtractionInvalidTest::RunTest(const FString& Parameters)
{
	TInstancedStruct<FPoint> EmptyAction;

	int32		  NumBranches = 3;
	TArray<float> BranchScores;
	float		  Confidence = 0.0f;

	int32 Result = RLDecisionHelpers::ExtractBranchIndexFromAction(EmptyAction, NumBranches, BranchScores, Confidence);

	TestEqual(TEXT("Invalid action should return -1"), Result, -1);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNumBranchesDiscreteSpaceTest, "Schola.StateTree.Helpers.NumBranches.DiscreteSpace", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FNumBranchesDiscreteSpaceTest::RunTest(const FString& Parameters)
{
	TInstancedStruct<FSpace> Space = TInstancedStruct<FSpace>::Make<FDiscreteSpace>(5);

	int32 NumBranches = RLDecisionHelpers::GetNumBranchesFromActionSpace(Space);

	TestEqual(TEXT("DiscreteSpace should return High value"), NumBranches, 5);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNumBranchesNonDiscreteSpaceTest, "Schola.StateTree.Helpers.NumBranches.NonDiscrete", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FNumBranchesNonDiscreteSpaceTest::RunTest(const FString& Parameters)
{
	// Non-discrete spaces return 0 (only FDiscreteSpace is valid for branch selection)
	TArray<float>			 Low = { -1.0f, -1.0f, -1.0f };
	TArray<float>			 High = { 1.0f, 1.0f, 1.0f };
	TInstancedStruct<FSpace> Space = TInstancedStruct<FSpace>::Make<FBoxSpace>(Low, High);

	int32 NumBranches = RLDecisionHelpers::GetNumBranchesFromActionSpace(Space);

	TestEqual(TEXT("Non-discrete space should return 0"), NumBranches, 0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNumBranchesInvalidSpaceTest, "Schola.StateTree.Helpers.NumBranches.Invalid", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FNumBranchesInvalidSpaceTest::RunTest(const FString& Parameters)
{
	TInstancedStruct<FSpace> EmptySpace;

	int32 NumBranches = RLDecisionHelpers::GetNumBranchesFromActionSpace(EmptySpace);

	TestEqual(TEXT("Invalid space should return 0"), NumBranches, 0);

	return true;
}

// ========== Edge Case Tests ==========

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBranchExtractionDiscretePointZeroTest, "Schola.StateTree.Helpers.BranchExtraction.DiscretePointZero", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FBranchExtractionDiscretePointZeroTest::RunTest(const FString& Parameters)
{
	TInstancedStruct<FPoint> Action = TInstancedStruct<FPoint>::Make<FDiscretePoint>(0);

	int32		  NumBranches = 3;
	TArray<float> BranchScores;
	float		  Confidence = 0.0f;

	int32 Result = RLDecisionHelpers::ExtractBranchIndexFromAction(Action, NumBranches, BranchScores, Confidence);

	TestEqual(TEXT("DiscretePoint with value 0 should return 0"), Result, 0);
	TestEqual(TEXT("Score for branch 0 should be 1.0"), BranchScores[0], 1.0f);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBranchExtractionBoxPointTiedValuesTest, "Schola.StateTree.Helpers.BranchExtraction.BoxPointTiedValues", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FBranchExtractionBoxPointTiedValuesTest::RunTest(const FString& Parameters)
{
	FBoxPoint BoxPoint;
	BoxPoint.Values = { 0.5f, 0.5f, 0.5f };
	TInstancedStruct<FPoint> Action = TInstancedStruct<FPoint>::Make<FBoxPoint>(BoxPoint);

	int32		  NumBranches = 0;
	TArray<float> BranchScores;
	float		  Confidence = 0.0f;

	int32 Result = RLDecisionHelpers::ExtractBranchIndexFromAction(Action, NumBranches, BranchScores, Confidence);

	// First max should be returned (index 0)
	TestEqual(TEXT("Tied values should return first max index"), Result, 0);
	TestEqual(TEXT("Confidence should be the tied value"), Confidence, 0.5f);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBranchExtractionBoxPointNegativeValuesTest, "Schola.StateTree.Helpers.BranchExtraction.BoxPointNegativeValues", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FBranchExtractionBoxPointNegativeValuesTest::RunTest(const FString& Parameters)
{
	FBoxPoint BoxPoint;
	BoxPoint.Values = { -0.5f, -0.1f, -0.9f };
	TInstancedStruct<FPoint> Action = TInstancedStruct<FPoint>::Make<FBoxPoint>(BoxPoint);

	int32		  NumBranches = 0;
	TArray<float> BranchScores;
	float		  Confidence = 0.0f;

	int32 Result = RLDecisionHelpers::ExtractBranchIndexFromAction(Action, NumBranches, BranchScores, Confidence);

	// -0.1 is the highest value
	TestEqual(TEXT("Negative values should still find argmax (index 1)"), Result, 1);
	TestEqual(TEXT("Confidence should be -0.1"), Confidence, -0.1f);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBranchExtractionBoxPointSingleValueTest, "Schola.StateTree.Helpers.BranchExtraction.BoxPointSingleValue", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FBranchExtractionBoxPointSingleValueTest::RunTest(const FString& Parameters)
{
	FBoxPoint BoxPoint;
	BoxPoint.Values = { 0.75f };
	TInstancedStruct<FPoint> Action = TInstancedStruct<FPoint>::Make<FBoxPoint>(BoxPoint);

	int32		  NumBranches = 0;
	TArray<float> BranchScores;
	float		  Confidence = 0.0f;

	int32 Result = RLDecisionHelpers::ExtractBranchIndexFromAction(Action, NumBranches, BranchScores, Confidence);

	TestEqual(TEXT("Single value BoxPoint should return index 0"), Result, 0);
	TestEqual(TEXT("NumBranches should be 1"), NumBranches, 1);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBranchExtractionEmptyDictPointTest, "Schola.StateTree.Helpers.BranchExtraction.EmptyDictPoint", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FBranchExtractionEmptyDictPointTest::RunTest(const FString& Parameters)
{
	FDictPoint DictPoint;
	// Empty dict - no entries
	TInstancedStruct<FPoint> Action = TInstancedStruct<FPoint>::Make<FDictPoint>(DictPoint);

	int32		  NumBranches = 3;
	TArray<float> BranchScores;
	float		  Confidence = 0.0f;

	int32 Result = RLDecisionHelpers::ExtractBranchIndexFromAction(Action, NumBranches, BranchScores, Confidence);

	TestEqual(TEXT("Empty DictPoint should return -1"), Result, -1);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBranchExtractionMultiDiscretePointTest, "Schola.StateTree.Helpers.BranchExtraction.MultiDiscretePoint", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FBranchExtractionMultiDiscretePointTest::RunTest(const FString& Parameters)
{
	FMultiDiscretePoint MultiDiscrete;
	MultiDiscrete.Values = { 3, 1, 2 };
	TInstancedStruct<FPoint> Action = TInstancedStruct<FPoint>::Make<FMultiDiscretePoint>(MultiDiscrete);

	int32		  NumBranches = 5;
	TArray<float> BranchScores;
	float		  Confidence = 0.0f;

	int32 Result = RLDecisionHelpers::ExtractBranchIndexFromAction(Action, NumBranches, BranchScores, Confidence);

	// MultiDiscretePoint is not supported for branch selection - only Discrete
	TestEqual(TEXT("MultiDiscretePoint should return -1 (unsupported)"), Result, -1);

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
