// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "Spaces/Blueprint/DictSpaceBlueprintLibrary.h"
#include "Spaces/Blueprint/DiscreteSpaceBlueprintLibrary.h"
#include "Spaces/Blueprint/BoxSpaceBlueprintLibrary.h"
#include "Spaces/DictSpace.h"
#include "Spaces/DiscreteSpace.h"
#include "Spaces/BoxSpace.h"
#include "Common/InstancedStructUtils.h"

#if WITH_DEV_AUTOMATION_TESTS

// MapToDictSpace Tests

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDictSpaceBlueprintLibrary_MapToDictSpace_BasicTest, "Schola.Spaces.Blueprint.DictSpaceBlueprintLibrary.MapToDictSpace.Basic", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FDictSpaceBlueprintLibrary_MapToDictSpace_BasicTest::RunTest(const FString& Parameters)
{
    TMap<FString, FInstancedStruct> Spaces;
    
    TInstancedStruct<FDiscreteSpace> DiscreteSpace = UDiscreteSpaceBlueprintLibrary::Int32ToDiscreteSpace(5);
    Spaces.Add(TEXT("action"), ToUntypedInstancedStruct(DiscreteSpace));

    TInstancedStruct<FDictSpace> Result = UDictSpaceBlueprintLibrary::MapToDictSpace(Spaces);

    TestTrue(TEXT("Result is valid"), Result.IsValid());
    
    const FDictSpace& DictSpace = Result.Get<FDictSpace>();
    TestEqual(TEXT("DictSpace.Spaces.Num() == 1"), DictSpace.Spaces.Num(), 1);
    TestTrue(TEXT("DictSpace contains 'action'"), DictSpace.Spaces.Contains(TEXT("action")));

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDictSpaceBlueprintLibrary_MapToDictSpace_EmptyTest, "Schola.Spaces.Blueprint.DictSpaceBlueprintLibrary.MapToDictSpace.Empty", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FDictSpaceBlueprintLibrary_MapToDictSpace_EmptyTest::RunTest(const FString& Parameters)
{
    TMap<FString, FInstancedStruct> Spaces;

    TInstancedStruct<FDictSpace> Result = UDictSpaceBlueprintLibrary::MapToDictSpace(Spaces);

    TestTrue(TEXT("Result is valid"), Result.IsValid());
    
    const FDictSpace& DictSpace = Result.Get<FDictSpace>();
    TestEqual(TEXT("DictSpace.Spaces.Num() == 0"), DictSpace.Spaces.Num(), 0);

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDictSpaceBlueprintLibrary_MapToDictSpace_MultipleTest, "Schola.Spaces.Blueprint.DictSpaceBlueprintLibrary.MapToDictSpace.Multiple", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FDictSpaceBlueprintLibrary_MapToDictSpace_MultipleTest::RunTest(const FString& Parameters)
{
    TMap<FString, FInstancedStruct> Spaces;
    
    TInstancedStruct<FDiscreteSpace> DiscreteSpace = UDiscreteSpaceBlueprintLibrary::Int32ToDiscreteSpace(5);
    Spaces.Add(TEXT("action"), ToUntypedInstancedStruct(DiscreteSpace));
    
    TArray<float> Low = {-1.0f, -1.0f};
    TArray<float> High = {1.0f, 1.0f};
    TArray<int32> Shape = {2};
    TInstancedStruct<FBoxSpace> BoxSpace = UBoxSpaceBlueprintLibrary::ArraysToBoxSpace(Low, High, Shape);
    Spaces.Add(TEXT("observation"), ToUntypedInstancedStruct(BoxSpace));

    TInstancedStruct<FDictSpace> Result = UDictSpaceBlueprintLibrary::MapToDictSpace(Spaces);

    TestTrue(TEXT("Result is valid"), Result.IsValid());
    
    const FDictSpace& DictSpace = Result.Get<FDictSpace>();
    TestEqual(TEXT("DictSpace.Spaces.Num() == 2"), DictSpace.Spaces.Num(), 2);
    TestTrue(TEXT("DictSpace contains 'action'"), DictSpace.Spaces.Contains(TEXT("action")));
    TestTrue(TEXT("DictSpace contains 'observation'"), DictSpace.Spaces.Contains(TEXT("observation")));

    return true;
}

// DictSpaceToMap Tests

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDictSpaceBlueprintLibrary_DictSpaceToMap_BasicTest, "Schola.Spaces.Blueprint.DictSpaceBlueprintLibrary.DictSpaceToMap.Basic", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FDictSpaceBlueprintLibrary_DictSpaceToMap_BasicTest::RunTest(const FString& Parameters)
{
    TInstancedStruct<FDictSpace> Space;
    Space.InitializeAs<FDictSpace>();
    FDictSpace& DictSpace = Space.GetMutable<FDictSpace>();
    
    TInstancedStruct<FDiscreteSpace> DiscreteSpace;
    DiscreteSpace.InitializeAs<FDiscreteSpace>(10);
    DictSpace.Spaces.Add(TEXT("test"), DiscreteSpace);

    TMap<FString, FInstancedStruct> Result = UDictSpaceBlueprintLibrary::DictSpaceToMap(Space);

    TestEqual(TEXT("Result.Num() == 1"), Result.Num(), 1);
    TestTrue(TEXT("Result contains 'test'"), Result.Contains(TEXT("test")));

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDictSpaceBlueprintLibrary_DictSpaceToMap_RoundTripTest, "Schola.Spaces.Blueprint.DictSpaceBlueprintLibrary.DictSpaceToMap.RoundTrip", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FDictSpaceBlueprintLibrary_DictSpaceToMap_RoundTripTest::RunTest(const FString& Parameters)
{
    TMap<FString, FInstancedStruct> OriginalSpaces;
    
    TInstancedStruct<FDiscreteSpace> DiscreteSpace = UDiscreteSpaceBlueprintLibrary::Int32ToDiscreteSpace(7);
    OriginalSpaces.Add(TEXT("key1"), ToUntypedInstancedStruct(DiscreteSpace));

    TInstancedStruct<FDictSpace> DictSpace = UDictSpaceBlueprintLibrary::MapToDictSpace(OriginalSpaces);
    TMap<FString, FInstancedStruct> Result = UDictSpaceBlueprintLibrary::DictSpaceToMap(DictSpace);

    TestEqual(TEXT("Round trip map size"), Result.Num(), OriginalSpaces.Num());
    TestTrue(TEXT("Round trip contains 'key1'"), Result.Contains(TEXT("key1")));

    return true;
}

// DictSpace_Add Tests

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDictSpaceBlueprintLibrary_Add_BasicTest, "Schola.Spaces.Blueprint.DictSpaceBlueprintLibrary.Add.Basic", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FDictSpaceBlueprintLibrary_Add_BasicTest::RunTest(const FString& Parameters)
{
    TInstancedStruct<FDictSpace> DictSpace;
    DictSpace.InitializeAs<FDictSpace>();

    TInstancedStruct<FDiscreteSpace> DiscreteSpace = UDiscreteSpaceBlueprintLibrary::Int32ToDiscreteSpace(5);

    bool Result = UDictSpaceBlueprintLibrary::DictSpace_Add(DictSpace, TEXT("action"), ToUntypedInstancedStruct(DiscreteSpace));

    TestTrue(TEXT("Add returned true"), Result);
    TestEqual(TEXT("DictSpace now has 1 element"), DictSpace.Get<FDictSpace>().Spaces.Num(), 1);

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDictSpaceBlueprintLibrary_Add_UpdateTest, "Schola.Spaces.Blueprint.DictSpaceBlueprintLibrary.Add.Update", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FDictSpaceBlueprintLibrary_Add_UpdateTest::RunTest(const FString& Parameters)
{
    TInstancedStruct<FDictSpace> DictSpace;
    DictSpace.InitializeAs<FDictSpace>();

    TInstancedStruct<FDiscreteSpace> DiscreteSpace1 = UDiscreteSpaceBlueprintLibrary::Int32ToDiscreteSpace(5);
    UDictSpaceBlueprintLibrary::DictSpace_Add(DictSpace, TEXT("action"), ToUntypedInstancedStruct(DiscreteSpace1));

    TInstancedStruct<FDiscreteSpace> DiscreteSpace2 = UDiscreteSpaceBlueprintLibrary::Int32ToDiscreteSpace(10);
    bool Result = UDictSpaceBlueprintLibrary::DictSpace_Add(DictSpace, TEXT("action"), ToUntypedInstancedStruct(DiscreteSpace2));

    TestTrue(TEXT("Update returned true"), Result);
    TestEqual(TEXT("DictSpace still has 1 element"), DictSpace.Get<FDictSpace>().Spaces.Num(), 1);

    return true;
}

// DictSpace_Find Tests

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDictSpaceBlueprintLibrary_Find_SuccessTest, "Schola.Spaces.Blueprint.DictSpaceBlueprintLibrary.Find.Success", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FDictSpaceBlueprintLibrary_Find_SuccessTest::RunTest(const FString& Parameters)
{
    TInstancedStruct<FDictSpace> DictSpace;
    DictSpace.InitializeAs<FDictSpace>();

    TInstancedStruct<FDiscreteSpace> DiscreteSpace = UDiscreteSpaceBlueprintLibrary::Int32ToDiscreteSpace(5);
    UDictSpaceBlueprintLibrary::DictSpace_Add(DictSpace, TEXT("action"), ToUntypedInstancedStruct(DiscreteSpace));

    FInstancedStruct OutValue;
    bool Result = UDictSpaceBlueprintLibrary::DictSpace_Find(DictSpace, TEXT("action"), OutValue);

    TestTrue(TEXT("Find returned true"), Result);
    TestTrue(TEXT("OutValue is valid"), OutValue.IsValid());

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDictSpaceBlueprintLibrary_Find_FailureTest, "Schola.Spaces.Blueprint.DictSpaceBlueprintLibrary.Find.Failure", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FDictSpaceBlueprintLibrary_Find_FailureTest::RunTest(const FString& Parameters)
{
    TInstancedStruct<FDictSpace> DictSpace;
    DictSpace.InitializeAs<FDictSpace>();

    FInstancedStruct OutValue;
    bool Result = UDictSpaceBlueprintLibrary::DictSpace_Find(DictSpace, TEXT("nonexistent"), OutValue);

    TestFalse(TEXT("Find returned false"), Result);

    return true;
}

// DictSpace_Contains Tests

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDictSpaceBlueprintLibrary_Contains_TrueTest, "Schola.Spaces.Blueprint.DictSpaceBlueprintLibrary.Contains.True", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FDictSpaceBlueprintLibrary_Contains_TrueTest::RunTest(const FString& Parameters)
{
    TInstancedStruct<FDictSpace> DictSpace;
    DictSpace.InitializeAs<FDictSpace>();

    TInstancedStruct<FDiscreteSpace> DiscreteSpace = UDiscreteSpaceBlueprintLibrary::Int32ToDiscreteSpace(5);
    UDictSpaceBlueprintLibrary::DictSpace_Add(DictSpace, TEXT("action"), ToUntypedInstancedStruct(DiscreteSpace));

    bool Result = UDictSpaceBlueprintLibrary::DictSpace_Contains(DictSpace, TEXT("action"));

    TestTrue(TEXT("Contains returned true"), Result);

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDictSpaceBlueprintLibrary_Contains_FalseTest, "Schola.Spaces.Blueprint.DictSpaceBlueprintLibrary.Contains.False", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FDictSpaceBlueprintLibrary_Contains_FalseTest::RunTest(const FString& Parameters)
{
    TInstancedStruct<FDictSpace> DictSpace;
    DictSpace.InitializeAs<FDictSpace>();

    bool Result = UDictSpaceBlueprintLibrary::DictSpace_Contains(DictSpace, TEXT("nonexistent"));

    TestFalse(TEXT("Contains returned false"), Result);

    return true;
}

// DictSpace_Remove Tests

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDictSpaceBlueprintLibrary_Remove_SuccessTest, "Schola.Spaces.Blueprint.DictSpaceBlueprintLibrary.Remove.Success", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FDictSpaceBlueprintLibrary_Remove_SuccessTest::RunTest(const FString& Parameters)
{
    TInstancedStruct<FDictSpace> DictSpace;
    DictSpace.InitializeAs<FDictSpace>();

    TInstancedStruct<FDiscreteSpace> DiscreteSpace = UDiscreteSpaceBlueprintLibrary::Int32ToDiscreteSpace(5);
    UDictSpaceBlueprintLibrary::DictSpace_Add(DictSpace, TEXT("action"), ToUntypedInstancedStruct(DiscreteSpace));

    bool Result = UDictSpaceBlueprintLibrary::DictSpace_Remove(DictSpace, TEXT("action"));

    TestTrue(TEXT("Remove returned true"), Result);
    TestEqual(TEXT("DictSpace now has 0 elements"), DictSpace.Get<FDictSpace>().Spaces.Num(), 0);

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDictSpaceBlueprintLibrary_Remove_FailureTest, "Schola.Spaces.Blueprint.DictSpaceBlueprintLibrary.Remove.Failure", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FDictSpaceBlueprintLibrary_Remove_FailureTest::RunTest(const FString& Parameters)
{
    TInstancedStruct<FDictSpace> DictSpace;
    DictSpace.InitializeAs<FDictSpace>();

    bool Result = UDictSpaceBlueprintLibrary::DictSpace_Remove(DictSpace, TEXT("nonexistent"));

    TestFalse(TEXT("Remove returned false"), Result);

    return true;
}

// DictSpace_Length Tests

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDictSpaceBlueprintLibrary_Length_EmptyTest, "Schola.Spaces.Blueprint.DictSpaceBlueprintLibrary.Length.Empty", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FDictSpaceBlueprintLibrary_Length_EmptyTest::RunTest(const FString& Parameters)
{
    TInstancedStruct<FDictSpace> DictSpace;
    DictSpace.InitializeAs<FDictSpace>();

    int32 Result = UDictSpaceBlueprintLibrary::DictSpace_Length(DictSpace);

    TestEqual(TEXT("Length == 0"), Result, 0);

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDictSpaceBlueprintLibrary_Length_MultipleTest, "Schola.Spaces.Blueprint.DictSpaceBlueprintLibrary.Length.Multiple", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FDictSpaceBlueprintLibrary_Length_MultipleTest::RunTest(const FString& Parameters)
{
    TInstancedStruct<FDictSpace> DictSpace;
    DictSpace.InitializeAs<FDictSpace>();

    TInstancedStruct<FDiscreteSpace> DiscreteSpace1 = UDiscreteSpaceBlueprintLibrary::Int32ToDiscreteSpace(5);
    TInstancedStruct<FDiscreteSpace> DiscreteSpace2 = UDiscreteSpaceBlueprintLibrary::Int32ToDiscreteSpace(10);
    TInstancedStruct<FDiscreteSpace> DiscreteSpace3 = UDiscreteSpaceBlueprintLibrary::Int32ToDiscreteSpace(15);

    UDictSpaceBlueprintLibrary::DictSpace_Add(DictSpace, TEXT("key1"), ToUntypedInstancedStruct(DiscreteSpace1));
    UDictSpaceBlueprintLibrary::DictSpace_Add(DictSpace, TEXT("key2"), ToUntypedInstancedStruct(DiscreteSpace2));
    UDictSpaceBlueprintLibrary::DictSpace_Add(DictSpace, TEXT("key3"), ToUntypedInstancedStruct(DiscreteSpace3));

    int32 Result = UDictSpaceBlueprintLibrary::DictSpace_Length(DictSpace);

    TestEqual(TEXT("Length == 3"), Result, 3);

    return true;
}

// DictSpace_Clear Tests

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDictSpaceBlueprintLibrary_Clear_BasicTest, "Schola.Spaces.Blueprint.DictSpaceBlueprintLibrary.Clear.Basic", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FDictSpaceBlueprintLibrary_Clear_BasicTest::RunTest(const FString& Parameters)
{
    TInstancedStruct<FDictSpace> DictSpace;
    DictSpace.InitializeAs<FDictSpace>();

    TInstancedStruct<FDiscreteSpace> DiscreteSpace1 = UDiscreteSpaceBlueprintLibrary::Int32ToDiscreteSpace(5);
    TInstancedStruct<FDiscreteSpace> DiscreteSpace2 = UDiscreteSpaceBlueprintLibrary::Int32ToDiscreteSpace(10);

    UDictSpaceBlueprintLibrary::DictSpace_Add(DictSpace, TEXT("key1"), ToUntypedInstancedStruct(DiscreteSpace1));
    UDictSpaceBlueprintLibrary::DictSpace_Add(DictSpace, TEXT("key2"), ToUntypedInstancedStruct(DiscreteSpace2));

    UDictSpaceBlueprintLibrary::DictSpace_Clear(DictSpace);

    TestEqual(TEXT("DictSpace is now empty"), DictSpace.Get<FDictSpace>().Spaces.Num(), 0);

    return true;
}

// DictSpace_Keys Tests

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDictSpaceBlueprintLibrary_Keys_BasicTest, "Schola.Spaces.Blueprint.DictSpaceBlueprintLibrary.Keys.Basic", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FDictSpaceBlueprintLibrary_Keys_BasicTest::RunTest(const FString& Parameters)
{
    TInstancedStruct<FDictSpace> DictSpace;
    DictSpace.InitializeAs<FDictSpace>();

    TInstancedStruct<FDiscreteSpace> DiscreteSpace1 = UDiscreteSpaceBlueprintLibrary::Int32ToDiscreteSpace(5);
    TInstancedStruct<FDiscreteSpace> DiscreteSpace2 = UDiscreteSpaceBlueprintLibrary::Int32ToDiscreteSpace(10);

    UDictSpaceBlueprintLibrary::DictSpace_Add(DictSpace, TEXT("action"), ToUntypedInstancedStruct(DiscreteSpace1));
    UDictSpaceBlueprintLibrary::DictSpace_Add(DictSpace, TEXT("observation"), ToUntypedInstancedStruct(DiscreteSpace2));

    TArray<FString> Keys;
    UDictSpaceBlueprintLibrary::DictSpace_Keys(DictSpace, Keys);

    TestEqual(TEXT("Keys.Num() == 2"), Keys.Num(), 2);
    TestTrue(TEXT("Keys contains 'action'"), Keys.Contains(TEXT("action")));
    TestTrue(TEXT("Keys contains 'observation'"), Keys.Contains(TEXT("observation")));

    return true;
}

// DictSpace_Values Tests

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDictSpaceBlueprintLibrary_Values_BasicTest, "Schola.Spaces.Blueprint.DictSpaceBlueprintLibrary.Values.Basic", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FDictSpaceBlueprintLibrary_Values_BasicTest::RunTest(const FString& Parameters)
{
    TInstancedStruct<FDictSpace> DictSpace;
    DictSpace.InitializeAs<FDictSpace>();

    TInstancedStruct<FDiscreteSpace> DiscreteSpace1 = UDiscreteSpaceBlueprintLibrary::Int32ToDiscreteSpace(5);
    TInstancedStruct<FDiscreteSpace> DiscreteSpace2 = UDiscreteSpaceBlueprintLibrary::Int32ToDiscreteSpace(10);
    
    UDictSpaceBlueprintLibrary::DictSpace_Add(DictSpace, TEXT("action"), ToUntypedInstancedStruct(DiscreteSpace1));
    UDictSpaceBlueprintLibrary::DictSpace_Add(DictSpace, TEXT("observation"), ToUntypedInstancedStruct(DiscreteSpace2));

    TArray<FInstancedStruct> Values;
    UDictSpaceBlueprintLibrary::DictSpace_Values(DictSpace, Values);

    TestEqual(TEXT("Values.Num() == 2"), Values.Num(), 2);
    TestTrue(TEXT("Values[0] is valid"), Values[0].IsValid());
    TestTrue(TEXT("Values[1] is valid"), Values[1].IsValid());

    return true;
}

#endif




