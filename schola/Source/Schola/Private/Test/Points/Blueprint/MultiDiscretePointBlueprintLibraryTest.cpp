// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "Points/Blueprint/MultiDiscretePointBlueprintLibrary.h"
#include "Points/MultiDiscretePoint.h"

#if WITH_DEV_AUTOMATION_TESTS

// ArrayToMultiDiscretePoint Tests

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMultiDiscretePointBlueprintLibrary_ArrayToMultiDiscretePoint_BasicTest, "Schola.Points.Blueprint.MultiDiscretePointBlueprintLibrary.ArrayToMultiDiscretePoint.Basic", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FMultiDiscretePointBlueprintLibrary_ArrayToMultiDiscretePoint_BasicTest::RunTest(const FString& Parameters)
{
    TArray<int32> Values = {1, 2, 3};

    TInstancedStruct<FMultiDiscretePoint> Result = UMultiDiscretePointBlueprintLibrary::ArrayToMultiDiscretePoint(Values);

    TestTrue(TEXT("Result is valid"), Result.IsValid());
    
    const FMultiDiscretePoint& MultiDiscretePoint = Result.Get<FMultiDiscretePoint>();
    TestEqual(TEXT("MultiDiscretePoint.Values"), MultiDiscretePoint.Values, Values);

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMultiDiscretePointBlueprintLibrary_ArrayToMultiDiscretePoint_EmptyTest, "Schola.Points.Blueprint.MultiDiscretePointBlueprintLibrary.ArrayToMultiDiscretePoint.Empty", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FMultiDiscretePointBlueprintLibrary_ArrayToMultiDiscretePoint_EmptyTest::RunTest(const FString& Parameters)
{
    TArray<int32> Values;

    TInstancedStruct<FMultiDiscretePoint> Result = UMultiDiscretePointBlueprintLibrary::ArrayToMultiDiscretePoint(Values);

    TestTrue(TEXT("Result is valid"), Result.IsValid());
    
    const FMultiDiscretePoint& MultiDiscretePoint = Result.Get<FMultiDiscretePoint>();
    TestEqual(TEXT("MultiDiscretePoint.Values.Num() == 0"), MultiDiscretePoint.Values.Num(), 0);

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMultiDiscretePointBlueprintLibrary_ArrayToMultiDiscretePoint_SingleTest, "Schola.Points.Blueprint.MultiDiscretePointBlueprintLibrary.ArrayToMultiDiscretePoint.Single", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FMultiDiscretePointBlueprintLibrary_ArrayToMultiDiscretePoint_SingleTest::RunTest(const FString& Parameters)
{
    TArray<int32> Values = {42};

    TInstancedStruct<FMultiDiscretePoint> Result = UMultiDiscretePointBlueprintLibrary::ArrayToMultiDiscretePoint(Values);

    TestTrue(TEXT("Result is valid"), Result.IsValid());
    
    const FMultiDiscretePoint& MultiDiscretePoint = Result.Get<FMultiDiscretePoint>();
    TestEqual(TEXT("MultiDiscretePoint.Values"), MultiDiscretePoint.Values, Values);

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMultiDiscretePointBlueprintLibrary_ArrayToMultiDiscretePoint_LargeTest, "Schola.Points.Blueprint.MultiDiscretePointBlueprintLibrary.ArrayToMultiDiscretePoint.Large", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FMultiDiscretePointBlueprintLibrary_ArrayToMultiDiscretePoint_LargeTest::RunTest(const FString& Parameters)
{
    TArray<int32> Values = {10, 20, 30, 40, 50, 60, 70, 80};

    TInstancedStruct<FMultiDiscretePoint> Result = UMultiDiscretePointBlueprintLibrary::ArrayToMultiDiscretePoint(Values);

    TestTrue(TEXT("Result is valid"), Result.IsValid());
    
    const FMultiDiscretePoint& MultiDiscretePoint = Result.Get<FMultiDiscretePoint>();
    TestEqual(TEXT("MultiDiscretePoint.Values"), MultiDiscretePoint.Values, Values);

    return true;
}

// MultiDiscretePointToArray Tests

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMultiDiscretePointBlueprintLibrary_MultiDiscretePointToArray_BasicTest, "Schola.Points.Blueprint.MultiDiscretePointBlueprintLibrary.MultiDiscretePointToArray.Basic", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FMultiDiscretePointBlueprintLibrary_MultiDiscretePointToArray_BasicTest::RunTest(const FString& Parameters)
{
    TInstancedStruct<FMultiDiscretePoint> Point;
    Point.InitializeAs<FMultiDiscretePoint>();
    Point.GetMutable<FMultiDiscretePoint>().Values = {5, 10, 15};

    TArray<int32> Result = UMultiDiscretePointBlueprintLibrary::MultiDiscretePointToArray(Point);

    TestEqual(TEXT("Result"), Result, TArray<int32>({5, 10, 15}));

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMultiDiscretePointBlueprintLibrary_MultiDiscretePointToArray_RoundTripTest, "Schola.Points.Blueprint.MultiDiscretePointBlueprintLibrary.MultiDiscretePointToArray.RoundTrip", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FMultiDiscretePointBlueprintLibrary_MultiDiscretePointToArray_RoundTripTest::RunTest(const FString& Parameters)
{
    TArray<int32> OriginalValues = {3, 6, 9, 12, 15};

    TInstancedStruct<FMultiDiscretePoint> Point = UMultiDiscretePointBlueprintLibrary::ArrayToMultiDiscretePoint(OriginalValues);
    TArray<int32> Result = UMultiDiscretePointBlueprintLibrary::MultiDiscretePointToArray(Point);

    TestEqual(TEXT("Round trip array"), Result, OriginalValues);

    return true;
}

#endif




