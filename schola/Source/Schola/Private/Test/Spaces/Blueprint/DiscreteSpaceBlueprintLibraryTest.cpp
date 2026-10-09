// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "Spaces/Blueprint/DiscreteSpaceBlueprintLibrary.h"
#include "Spaces/DiscreteSpace.h"

#if WITH_DEV_AUTOMATION_TESTS

// Int32ToDiscreteSpace Tests

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDiscreteSpaceBlueprintLibrary_Int32ToDiscreteSpace_BasicTest, "Schola.Spaces.Blueprint.DiscreteSpaceBlueprintLibrary.Int32ToDiscreteSpace.Basic", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FDiscreteSpaceBlueprintLibrary_Int32ToDiscreteSpace_BasicTest::RunTest(const FString& Parameters)
{
    int32 High = 10;

    TInstancedStruct<FDiscreteSpace> Result = UDiscreteSpaceBlueprintLibrary::Int32ToDiscreteSpace(High);

    TestTrue(TEXT("Result is valid"), Result.IsValid());
    
    const FDiscreteSpace& DiscreteSpace = Result.Get<FDiscreteSpace>();
    TestEqual(TEXT("DiscreteSpace.High == 10"), DiscreteSpace.High, 10);

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDiscreteSpaceBlueprintLibrary_Int32ToDiscreteSpace_ZeroTest, "Schola.Spaces.Blueprint.DiscreteSpaceBlueprintLibrary.Int32ToDiscreteSpace.Zero", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FDiscreteSpaceBlueprintLibrary_Int32ToDiscreteSpace_ZeroTest::RunTest(const FString& Parameters)
{
    int32 High = 0;

    TInstancedStruct<FDiscreteSpace> Result = UDiscreteSpaceBlueprintLibrary::Int32ToDiscreteSpace(High);

    TestTrue(TEXT("Result is valid"), Result.IsValid());
    
    const FDiscreteSpace& DiscreteSpace = Result.Get<FDiscreteSpace>();
    TestEqual(TEXT("DiscreteSpace.High == 0"), DiscreteSpace.High, 0);

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDiscreteSpaceBlueprintLibrary_Int32ToDiscreteSpace_LargeValueTest, "Schola.Spaces.Blueprint.DiscreteSpaceBlueprintLibrary.Int32ToDiscreteSpace.LargeValue", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FDiscreteSpaceBlueprintLibrary_Int32ToDiscreteSpace_LargeValueTest::RunTest(const FString& Parameters)
{
    int32 High = 1000000;

    TInstancedStruct<FDiscreteSpace> Result = UDiscreteSpaceBlueprintLibrary::Int32ToDiscreteSpace(High);

    TestTrue(TEXT("Result is valid"), Result.IsValid());
    
    const FDiscreteSpace& DiscreteSpace = Result.Get<FDiscreteSpace>();
    TestEqual(TEXT("DiscreteSpace.High == 1000000"), DiscreteSpace.High, 1000000);

    return true;
}

// DiscreteSpaceToInt32 Tests

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDiscreteSpaceBlueprintLibrary_DiscreteSpaceToInt32_BasicTest, "Schola.Spaces.Blueprint.DiscreteSpaceBlueprintLibrary.DiscreteSpaceToInt32.Basic", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FDiscreteSpaceBlueprintLibrary_DiscreteSpaceToInt32_BasicTest::RunTest(const FString& Parameters)
{
    TInstancedStruct<FDiscreteSpace> Space;
    Space.InitializeAs<FDiscreteSpace>(42);

    int32 Result = UDiscreteSpaceBlueprintLibrary::DiscreteSpaceToInt32(Space);

    TestEqual(TEXT("Result == 42"), Result, 42);

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDiscreteSpaceBlueprintLibrary_DiscreteSpaceToInt32_RoundTripTest, "Schola.Spaces.Blueprint.DiscreteSpaceBlueprintLibrary.DiscreteSpaceToInt32.RoundTrip", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FDiscreteSpaceBlueprintLibrary_DiscreteSpaceToInt32_RoundTripTest::RunTest(const FString& Parameters)
{
    int32 OriginalValue = 256;

    TInstancedStruct<FDiscreteSpace> Space = UDiscreteSpaceBlueprintLibrary::Int32ToDiscreteSpace(OriginalValue);
    int32 Result = UDiscreteSpaceBlueprintLibrary::DiscreteSpaceToInt32(Space);

    TestEqual(TEXT("Round trip value == 256"), Result, OriginalValue);

    return true;
}

#endif




