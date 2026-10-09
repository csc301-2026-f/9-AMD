// Copyright (c) 2026 Advanced Micro Devices, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"

#include "Conditions/StateTreeCondition_RLBranch.h"

#if WITH_DEV_AUTOMATION_TESTS

namespace RLBranchConditionTestHelpers
{
	bool TestBranchMatch(int32 SelectedBranch, float Confidence, int32 BranchIndex, bool bInvertCondition, float MinConfidence)
	{
		if (MinConfidence > 0.0f && Confidence < MinConfidence)
		{
			return bInvertCondition;
		}
		bool bBranchMatches = (SelectedBranch == BranchIndex);
		return bInvertCondition ? !bBranchMatches : bBranchMatches;
	}
} // namespace RLBranchConditionTestHelpers

// ========== Instance Data Tests ==========

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRLBranchInstanceDataTest, "Schola.StateTree.Conditions.RLBranch.InstanceData", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FRLBranchInstanceDataTest::RunTest(const FString& Parameters)
{
	FStateTreeCondition_RLBranchInstanceData InstanceData;

	TestEqual(TEXT("Default SelectedBranch should be -1"), InstanceData.SelectedBranch, -1);
	TestEqual(TEXT("Default Confidence should be 1.0"), InstanceData.Confidence, 1.0f);

	InstanceData.SelectedBranch = 2;
	InstanceData.Confidence = 0.85f;

	TestEqual(TEXT("SelectedBranch should be settable"), InstanceData.SelectedBranch, 2);
	TestEqual(TEXT("Confidence should be settable"), InstanceData.Confidence, 0.85f);

	return true;
}

// ========== Condition Defaults Tests ==========

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRLBranchConditionDefaultsTest, "Schola.StateTree.Conditions.RLBranch.Defaults", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FRLBranchConditionDefaultsTest::RunTest(const FString& Parameters)
{
	FStateTreeCondition_RLBranch Condition;

	TestEqual(TEXT("Default BranchIndex should be 0"), Condition.BranchIndex, 0);
	TestEqual(TEXT("Default bInvertCondition should be false"), Condition.bInvertCondition, false);
	TestEqual(TEXT("Default MinConfidence should be 0.0"), Condition.MinConfidence, 0.0f);

	return true;
}

// ========== Branch Matching Tests ==========

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRLBranchMatchTest, "Schola.StateTree.Conditions.RLBranch.BranchMatch", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FRLBranchMatchTest::RunTest(const FString& Parameters)
{
	using namespace RLBranchConditionTestHelpers;

	TestTrue(TEXT("Branch 1 should match BranchIndex 1"), TestBranchMatch(1, 1.0f, 1, false, 0.0f));
	TestFalse(TEXT("Branch 0 should not match BranchIndex 1"), TestBranchMatch(0, 1.0f, 1, false, 0.0f));
	TestTrue(TEXT("Branch 0 should match BranchIndex 0"), TestBranchMatch(0, 1.0f, 0, false, 0.0f));
	TestTrue(TEXT("Branch 2 should match BranchIndex 2"), TestBranchMatch(2, 1.0f, 2, false, 0.0f));

	return true;
}

// ========== Invert Tests ==========

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRLBranchInvertTest, "Schola.StateTree.Conditions.RLBranch.Invert", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FRLBranchInvertTest::RunTest(const FString& Parameters)
{
	using namespace RLBranchConditionTestHelpers;

	TestFalse(TEXT("Inverted: Branch 1 matching BranchIndex 1 should return false"), TestBranchMatch(1, 1.0f, 1, true, 0.0f));
	TestTrue(TEXT("Inverted: Branch 0 not matching BranchIndex 1 should return true"), TestBranchMatch(0, 1.0f, 1, true, 0.0f));

	return true;
}

// ========== Confidence Tests ==========

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRLBranchConfidenceTest, "Schola.StateTree.Conditions.RLBranch.Confidence", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FRLBranchConfidenceTest::RunTest(const FString& Parameters)
{
	using namespace RLBranchConditionTestHelpers;

	TestFalse(TEXT("Low confidence should fail with MinConfidence threshold"), TestBranchMatch(1, 0.5f, 1, false, 0.8f));
	TestTrue(TEXT("High confidence should pass with MinConfidence threshold"), TestBranchMatch(1, 0.9f, 1, false, 0.8f));
	TestTrue(TEXT("Inverted low confidence should pass"), TestBranchMatch(1, 0.5f, 1, true, 0.8f));

	return true;
}

// ========== Uninitialized State Tests ==========

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRLBranchUninitializedTest, "Schola.StateTree.Conditions.RLBranch.Uninitialized", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FRLBranchUninitializedTest::RunTest(const FString& Parameters)
{
	using namespace RLBranchConditionTestHelpers;

	TestFalse(TEXT("Uninitialized branch (-1) should not match any valid branch"), TestBranchMatch(-1, 1.0f, 0, false, 0.0f));
	TestFalse(TEXT("Uninitialized branch (-1) should not match branch 1"), TestBranchMatch(-1, 1.0f, 1, false, 0.0f));

	return true;
}

// ========== Edge Case Tests ==========

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRLBranchLargeBranchIndexTest, "Schola.StateTree.Conditions.RLBranch.LargeBranchIndex", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FRLBranchLargeBranchIndexTest::RunTest(const FString& Parameters)
{
	using namespace RLBranchConditionTestHelpers;

	// Test with large branch indices
	TestTrue(TEXT("Large branch index 100 should match"), TestBranchMatch(100, 1.0f, 100, false, 0.0f));
	TestFalse(TEXT("Large branch index 100 should not match 99"), TestBranchMatch(100, 1.0f, 99, false, 0.0f));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRLBranchExactConfidenceThresholdTest, "Schola.StateTree.Conditions.RLBranch.ExactConfidenceThreshold", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FRLBranchExactConfidenceThresholdTest::RunTest(const FString& Parameters)
{
	using namespace RLBranchConditionTestHelpers;

	// Confidence exactly at threshold should pass
	TestTrue(TEXT("Confidence exactly at threshold should pass"), TestBranchMatch(1, 0.8f, 1, false, 0.8f));

	// Confidence just below threshold should fail
	TestFalse(TEXT("Confidence just below threshold should fail"), TestBranchMatch(1, 0.79f, 1, false, 0.8f));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRLBranchZeroConfidenceTest, "Schola.StateTree.Conditions.RLBranch.ZeroConfidence", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FRLBranchZeroConfidenceTest::RunTest(const FString& Parameters)
{
	using namespace RLBranchConditionTestHelpers;

	// Zero confidence with no threshold should pass
	TestTrue(TEXT("Zero confidence with no threshold should pass if branch matches"), TestBranchMatch(1, 0.0f, 1, false, 0.0f));

	// Zero confidence with threshold should fail
	TestFalse(TEXT("Zero confidence with threshold should fail"), TestBranchMatch(1, 0.0f, 1, false, 0.5f));

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
