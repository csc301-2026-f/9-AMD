// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"


#if WITH_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBuildSucceedsTest, "Schola.Misc.Build Succeeds Test", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBuildSucceedsTest::RunTest(const FString& Parameters)
{
    // An empty test that can be used in external test platforms to check if the build suceeded.
    return true;
}

#endif