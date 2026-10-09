// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "Spaces/Blueprint/MultiBinarySpaceBlueprintLibrary.h"
#include "Spaces/MultiBinarySpace.h"

#if WITH_DEV_AUTOMATION_TESTS

// ShapeToMultiBinarySpace Tests

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMultiBinarySpaceBlueprintLibrary_ShapeToMultiBinarySpace_BasicTest, "Schola.Spaces.Blueprint.MultiBinarySpaceBlueprintLibrary.ShapeToMultiBinarySpace.Basic", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FMultiBinarySpaceBlueprintLibrary_ShapeToMultiBinarySpace_BasicTest::RunTest(const FString& Parameters)
{
    int32 Shape = 8;

    TInstancedStruct<FMultiBinarySpace> Result = UMultiBinarySpaceBlueprintLibrary::ShapeToMultiBinarySpace(Shape);

    TestTrue(TEXT("Result is valid"), Result.IsValid());
    
    const FMultiBinarySpace& MultiBinarySpace = Result.Get<FMultiBinarySpace>();
    TestEqual(TEXT("MultiBinarySpace.Shape == 8"), MultiBinarySpace.Shape, 8);

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMultiBinarySpaceBlueprintLibrary_ShapeToMultiBinarySpace_ZeroTest, "Schola.Spaces.Blueprint.MultiBinarySpaceBlueprintLibrary.ShapeToMultiBinarySpace.Zero", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FMultiBinarySpaceBlueprintLibrary_ShapeToMultiBinarySpace_ZeroTest::RunTest(const FString& Parameters)
{
    int32 Shape = 0;

    TInstancedStruct<FMultiBinarySpace> Result = UMultiBinarySpaceBlueprintLibrary::ShapeToMultiBinarySpace(Shape);

    TestTrue(TEXT("Result is valid"), Result.IsValid());
    
    const FMultiBinarySpace& MultiBinarySpace = Result.Get<FMultiBinarySpace>();
    TestEqual(TEXT("MultiBinarySpace.Shape == 0"), MultiBinarySpace.Shape, 0);

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMultiBinarySpaceBlueprintLibrary_ShapeToMultiBinarySpace_LargeTest, "Schola.Spaces.Blueprint.MultiBinarySpaceBlueprintLibrary.ShapeToMultiBinarySpace.Large", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FMultiBinarySpaceBlueprintLibrary_ShapeToMultiBinarySpace_LargeTest::RunTest(const FString& Parameters)
{
    int32 Shape = 1024;

    TInstancedStruct<FMultiBinarySpace> Result = UMultiBinarySpaceBlueprintLibrary::ShapeToMultiBinarySpace(Shape);

    TestTrue(TEXT("Result is valid"), Result.IsValid());
    
    const FMultiBinarySpace& MultiBinarySpace = Result.Get<FMultiBinarySpace>();
    TestEqual(TEXT("MultiBinarySpace.Shape == 1024"), MultiBinarySpace.Shape, 1024);

    return true;
}

// MultiBinarySpace_GetShape Tests

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMultiBinarySpaceBlueprintLibrary_GetShape_BasicTest, "Schola.Spaces.Blueprint.MultiBinarySpaceBlueprintLibrary.GetShape.Basic", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FMultiBinarySpaceBlueprintLibrary_GetShape_BasicTest::RunTest(const FString& Parameters)
{
    TInstancedStruct<FMultiBinarySpace> Space;
    Space.InitializeAs<FMultiBinarySpace>();
    Space.GetMutable<FMultiBinarySpace>().Shape = 16;

    int32 Result = UMultiBinarySpaceBlueprintLibrary::MultiBinarySpace_GetShape(Space);

    TestEqual(TEXT("Result == 16"), Result, 16);

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMultiBinarySpaceBlueprintLibrary_GetShape_RoundTripTest, "Schola.Spaces.Blueprint.MultiBinarySpaceBlueprintLibrary.GetShape.RoundTrip", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FMultiBinarySpaceBlueprintLibrary_GetShape_RoundTripTest::RunTest(const FString& Parameters)
{
    int32 OriginalShape = 32;

    TInstancedStruct<FMultiBinarySpace> Space = UMultiBinarySpaceBlueprintLibrary::ShapeToMultiBinarySpace(OriginalShape);
    int32 Result = UMultiBinarySpaceBlueprintLibrary::MultiBinarySpace_GetShape(Space);

    TestEqual(TEXT("Round trip shape == 32"), Result, OriginalShape);

    return true;
}

#endif




