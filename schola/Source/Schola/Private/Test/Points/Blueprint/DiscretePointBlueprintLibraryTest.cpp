// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "Points/Blueprint/DiscretePointBlueprintLibrary.h"
#include "Points/DiscretePoint.h"

#if WITH_DEV_AUTOMATION_TESTS

// Int32ToDiscretePoint Tests

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDiscretePointBlueprintLibrary_Int32ToDiscretePoint_BasicTest, "Schola.Points.Blueprint.DiscretePointBlueprintLibrary.Int32ToDiscretePoint.Basic", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FDiscretePointBlueprintLibrary_Int32ToDiscretePoint_BasicTest::RunTest(const FString& Parameters)
{
    int32 Value = 5;

    TInstancedStruct<FDiscretePoint> Result = UDiscretePointBlueprintLibrary::Int32ToDiscretePoint(Value);

    TestTrue(TEXT("Result is valid"), Result.IsValid());
    
    const FDiscretePoint& DiscretePoint = Result.Get<FDiscretePoint>();
    TestEqual(TEXT("DiscretePoint.Value == 5"), DiscretePoint.Value, 5);

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDiscretePointBlueprintLibrary_Int32ToDiscretePoint_ZeroTest, "Schola.Points.Blueprint.DiscretePointBlueprintLibrary.Int32ToDiscretePoint.Zero", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FDiscretePointBlueprintLibrary_Int32ToDiscretePoint_ZeroTest::RunTest(const FString& Parameters)
{
    int32 Value = 0;

    TInstancedStruct<FDiscretePoint> Result = UDiscretePointBlueprintLibrary::Int32ToDiscretePoint(Value);

    TestTrue(TEXT("Result is valid"), Result.IsValid());
    
    const FDiscretePoint& DiscretePoint = Result.Get<FDiscretePoint>();
    TestEqual(TEXT("DiscretePoint.Value == 0"), DiscretePoint.Value, 0);

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDiscretePointBlueprintLibrary_Int32ToDiscretePoint_LargeValueTest, "Schola.Points.Blueprint.DiscretePointBlueprintLibrary.Int32ToDiscretePoint.LargeValue", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FDiscretePointBlueprintLibrary_Int32ToDiscretePoint_LargeValueTest::RunTest(const FString& Parameters)
{
    int32 Value = 999999;

    TInstancedStruct<FDiscretePoint> Result = UDiscretePointBlueprintLibrary::Int32ToDiscretePoint(Value);

    TestTrue(TEXT("Result is valid"), Result.IsValid());
    
    const FDiscretePoint& DiscretePoint = Result.Get<FDiscretePoint>();
    TestEqual(TEXT("DiscretePoint.Value == 999999"), DiscretePoint.Value, 999999);

    return true;
}

// DiscretePointToInt32 Tests

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDiscretePointBlueprintLibrary_DiscretePointToInt32_BasicTest, "Schola.Points.Blueprint.DiscretePointBlueprintLibrary.DiscretePointToInt32.Basic", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FDiscretePointBlueprintLibrary_DiscretePointToInt32_BasicTest::RunTest(const FString& Parameters)
{
    TInstancedStruct<FDiscretePoint> Point;
    Point.InitializeAs<FDiscretePoint>(42);

    int32 Result = UDiscretePointBlueprintLibrary::DiscretePointToInt32(Point);

    TestEqual(TEXT("Result == 42"), Result, 42);

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDiscretePointBlueprintLibrary_DiscretePointToInt32_RoundTripTest, "Schola.Points.Blueprint.DiscretePointBlueprintLibrary.DiscretePointToInt32.RoundTrip", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FDiscretePointBlueprintLibrary_DiscretePointToInt32_RoundTripTest::RunTest(const FString& Parameters)
{
    int32 OriginalValue = 128;

    TInstancedStruct<FDiscretePoint> Point = UDiscretePointBlueprintLibrary::Int32ToDiscretePoint(OriginalValue);
    int32 Result = UDiscretePointBlueprintLibrary::DiscretePointToInt32(Point);

    TestEqual(TEXT("Round trip value == 128"), Result, OriginalValue);

    return true;
}

#endif




