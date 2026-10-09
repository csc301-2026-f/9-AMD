// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "Points/Blueprint/DictPointBlueprintLibrary.h"
#include "Points/Blueprint/DiscretePointBlueprintLibrary.h"
#include "Points/Blueprint/BoxPointBlueprintLibrary.h"
#include "Points/DictPoint.h"
#include "Points/DiscretePoint.h"
#include "Points/BoxPoint.h"
#include "Common/InstancedStructUtils.h"

#if WITH_DEV_AUTOMATION_TESTS

// MapToDictPoint Tests

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDictPointBlueprintLibrary_MapToDictPoint_BasicTest, "Schola.Points.Blueprint.DictPointBlueprintLibrary.MapToDictPoint.Basic", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FDictPointBlueprintLibrary_MapToDictPoint_BasicTest::RunTest(const FString& Parameters)
{
    TMap<FString, FInstancedStruct> Points;
    
    TInstancedStruct<FDiscretePoint> DiscretePoint = UDiscretePointBlueprintLibrary::Int32ToDiscretePoint(5);
	Points.Add(TEXT("action"), ToUntypedInstancedStruct(DiscretePoint));

    TInstancedStruct<FDictPoint> Result = UDictPointBlueprintLibrary::MapToDictPoint(Points);

    TestTrue(TEXT("Result is valid"), Result.IsValid());
    
    const FDictPoint& DictPoint = Result.Get<FDictPoint>();
    TestEqual(TEXT("DictPoint.Points.Num() == 1"), DictPoint.Points.Num(), 1);
    TestTrue(TEXT("DictPoint contains 'action'"), DictPoint.Points.Contains(TEXT("action")));

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDictPointBlueprintLibrary_MapToDictPoint_EmptyTest, "Schola.Points.Blueprint.DictPointBlueprintLibrary.MapToDictPoint.Empty", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FDictPointBlueprintLibrary_MapToDictPoint_EmptyTest::RunTest(const FString& Parameters)
{
    TMap<FString, FInstancedStruct> Points;

    TInstancedStruct<FDictPoint> Result = UDictPointBlueprintLibrary::MapToDictPoint(Points);

    TestTrue(TEXT("Result is valid"), Result.IsValid());
    
    const FDictPoint& DictPoint = Result.Get<FDictPoint>();
    TestEqual(TEXT("DictPoint.Points.Num() == 0"), DictPoint.Points.Num(), 0);

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDictPointBlueprintLibrary_MapToDictPoint_MultipleTest, "Schola.Points.Blueprint.DictPointBlueprintLibrary.MapToDictPoint.Multiple", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FDictPointBlueprintLibrary_MapToDictPoint_MultipleTest::RunTest(const FString& Parameters)
{
    TMap<FString, FInstancedStruct> Points;
    
    TInstancedStruct<FDiscretePoint> DiscretePoint = UDiscretePointBlueprintLibrary::Int32ToDiscretePoint(5);
	Points.Add(TEXT("action"), ToUntypedInstancedStruct(DiscretePoint));
    
    TArray<float> Values = {1.0f, 2.0f};
    TInstancedStruct<FBoxPoint> BoxPoint = UBoxPointBlueprintLibrary::ArrayToBoxPoint(Values);
	Points.Add(TEXT("observation"), ToUntypedInstancedStruct(BoxPoint));

    TInstancedStruct<FDictPoint> Result = UDictPointBlueprintLibrary::MapToDictPoint(Points);

    TestTrue(TEXT("Result is valid"), Result.IsValid());
    
    const FDictPoint& DictPoint = Result.Get<FDictPoint>();
    TestEqual(TEXT("DictPoint.Points.Num() == 2"), DictPoint.Points.Num(), 2);
    TestTrue(TEXT("DictPoint contains 'action'"), DictPoint.Points.Contains(TEXT("action")));
    TestTrue(TEXT("DictPoint contains 'observation'"), DictPoint.Points.Contains(TEXT("observation")));

    return true;
}

// DictPointToMap Tests

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDictPointBlueprintLibrary_DictPointToMap_BasicTest, "Schola.Points.Blueprint.DictPointBlueprintLibrary.DictPointToMap.Basic", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FDictPointBlueprintLibrary_DictPointToMap_BasicTest::RunTest(const FString& Parameters)
{
    TInstancedStruct<FDictPoint> Point;
    Point.InitializeAs<FDictPoint>();
    FDictPoint& DictPoint = Point.GetMutable<FDictPoint>();
    
    TInstancedStruct<FDiscretePoint> DiscretePoint;
    DiscretePoint.InitializeAs<FDiscretePoint>(10);
    DictPoint.Points.Add(TEXT("test"), DiscretePoint);

    TMap<FString, FInstancedStruct> Result = UDictPointBlueprintLibrary::DictPointToMap(Point);

    TestEqual(TEXT("Result.Num() == 1"), Result.Num(), 1);
    TestTrue(TEXT("Result contains 'test'"), Result.Contains(TEXT("test")));

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDictPointBlueprintLibrary_DictPointToMap_RoundTripTest, "Schola.Points.Blueprint.DictPointBlueprintLibrary.DictPointToMap.RoundTrip", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FDictPointBlueprintLibrary_DictPointToMap_RoundTripTest::RunTest(const FString& Parameters)
{
    TMap<FString, FInstancedStruct> OriginalPoints;
    
    TInstancedStruct<FDiscretePoint> DiscretePoint = UDiscretePointBlueprintLibrary::Int32ToDiscretePoint(7);
	OriginalPoints.Add(TEXT("key1"), ToUntypedInstancedStruct(DiscretePoint));

    TInstancedStruct<FDictPoint> DictPoint = UDictPointBlueprintLibrary::MapToDictPoint(OriginalPoints);
    TMap<FString, FInstancedStruct> Result = UDictPointBlueprintLibrary::DictPointToMap(DictPoint);

    TestEqual(TEXT("Round trip map size"), Result.Num(), OriginalPoints.Num());
    TestTrue(TEXT("Round trip contains 'key1'"), Result.Contains(TEXT("key1")));

    return true;
}

// DictPoint_Add Tests

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDictPointBlueprintLibrary_Add_BasicTest, "Schola.Points.Blueprint.DictPointBlueprintLibrary.Add.Basic", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FDictPointBlueprintLibrary_Add_BasicTest::RunTest(const FString& Parameters)
{
    TInstancedStruct<FDictPoint> DictPoint;
    DictPoint.InitializeAs<FDictPoint>();

    TInstancedStruct<FDiscretePoint> DiscretePoint = UDiscretePointBlueprintLibrary::Int32ToDiscretePoint(5);
    
    bool Result = UDictPointBlueprintLibrary::DictPoint_Add(DictPoint, TEXT("action"), ToUntypedInstancedStruct(DiscretePoint));

    TestTrue(TEXT("Add returned true"), Result);
    TestEqual(TEXT("DictPoint now has 1 element"), DictPoint.Get<FDictPoint>().Points.Num(), 1);

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDictPointBlueprintLibrary_Add_UpdateTest, "Schola.Points.Blueprint.DictPointBlueprintLibrary.Add.Update", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FDictPointBlueprintLibrary_Add_UpdateTest::RunTest(const FString& Parameters)
{
    TInstancedStruct<FDictPoint> DictPoint;
    DictPoint.InitializeAs<FDictPoint>();

    TInstancedStruct<FDiscretePoint> DiscretePoint1 = UDiscretePointBlueprintLibrary::Int32ToDiscretePoint(5);
    UDictPointBlueprintLibrary::DictPoint_Add(DictPoint, TEXT("action"), ToUntypedInstancedStruct(DiscretePoint1));

    TInstancedStruct<FDiscretePoint> DiscretePoint2 = UDiscretePointBlueprintLibrary::Int32ToDiscretePoint(10);
	bool Result = UDictPointBlueprintLibrary::DictPoint_Add(DictPoint, TEXT("action"), ToUntypedInstancedStruct(DiscretePoint2));

    TestTrue(TEXT("Update returned true"), Result);
    TestEqual(TEXT("DictPoint still has 1 element"), DictPoint.Get<FDictPoint>().Points.Num(), 1);

    return true;
}

// DictPoint_Find Tests

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDictPointBlueprintLibrary_Find_SuccessTest, "Schola.Points.Blueprint.DictPointBlueprintLibrary.Find.Success", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FDictPointBlueprintLibrary_Find_SuccessTest::RunTest(const FString& Parameters)
{
    TInstancedStruct<FDictPoint> DictPoint;
    DictPoint.InitializeAs<FDictPoint>();

    TInstancedStruct<FDiscretePoint> DiscretePoint = UDiscretePointBlueprintLibrary::Int32ToDiscretePoint(5);
    UDictPointBlueprintLibrary::DictPoint_Add(DictPoint, TEXT("action"), ToUntypedInstancedStruct(DiscretePoint));

    FInstancedStruct OutValue;
    bool Result = UDictPointBlueprintLibrary::DictPoint_Find(DictPoint, TEXT("action"), OutValue);

    TestTrue(TEXT("Find returned true"), Result);
    TestTrue(TEXT("OutValue is valid"), OutValue.IsValid());

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDictPointBlueprintLibrary_Find_FailureTest, "Schola.Points.Blueprint.DictPointBlueprintLibrary.Find.Failure", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FDictPointBlueprintLibrary_Find_FailureTest::RunTest(const FString& Parameters)
{
    TInstancedStruct<FDictPoint> DictPoint;
    DictPoint.InitializeAs<FDictPoint>();

    FInstancedStruct OutValue;
    bool Result = UDictPointBlueprintLibrary::DictPoint_Find(DictPoint, TEXT("nonexistent"), OutValue);

    TestFalse(TEXT("Find returned false"), Result);

    return true;
}

// DictPoint_Contains Tests

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDictPointBlueprintLibrary_Contains_TrueTest, "Schola.Points.Blueprint.DictPointBlueprintLibrary.Contains.True", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FDictPointBlueprintLibrary_Contains_TrueTest::RunTest(const FString& Parameters)
{
    TInstancedStruct<FDictPoint> DictPoint;
    DictPoint.InitializeAs<FDictPoint>();

    TInstancedStruct<FDiscretePoint> DiscretePoint = UDiscretePointBlueprintLibrary::Int32ToDiscretePoint(5);
    UDictPointBlueprintLibrary::DictPoint_Add(DictPoint, TEXT("action"), ToUntypedInstancedStruct(DiscretePoint));

    bool Result = UDictPointBlueprintLibrary::DictPoint_Contains(DictPoint, TEXT("action"));

    TestTrue(TEXT("Contains returned true"), Result);

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDictPointBlueprintLibrary_Contains_FalseTest, "Schola.Points.Blueprint.DictPointBlueprintLibrary.Contains.False", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FDictPointBlueprintLibrary_Contains_FalseTest::RunTest(const FString& Parameters)
{
    TInstancedStruct<FDictPoint> DictPoint;
    DictPoint.InitializeAs<FDictPoint>();

    bool Result = UDictPointBlueprintLibrary::DictPoint_Contains(DictPoint, TEXT("nonexistent"));

    TestFalse(TEXT("Contains returned false"), Result);

    return true;
}

// DictPoint_Remove Tests

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDictPointBlueprintLibrary_Remove_SuccessTest, "Schola.Points.Blueprint.DictPointBlueprintLibrary.Remove.Success", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FDictPointBlueprintLibrary_Remove_SuccessTest::RunTest(const FString& Parameters)
{
    TInstancedStruct<FDictPoint> DictPoint;
    DictPoint.InitializeAs<FDictPoint>();

    TInstancedStruct<FDiscretePoint> DiscretePoint = UDiscretePointBlueprintLibrary::Int32ToDiscretePoint(5);
	UDictPointBlueprintLibrary::DictPoint_Add(DictPoint, TEXT("action"), ToUntypedInstancedStruct(DiscretePoint));

    bool Result = UDictPointBlueprintLibrary::DictPoint_Remove(DictPoint, TEXT("action"));

    TestTrue(TEXT("Remove returned true"), Result);
    TestEqual(TEXT("DictPoint now has 0 elements"), DictPoint.Get<FDictPoint>().Points.Num(), 0);

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDictPointBlueprintLibrary_Remove_FailureTest, "Schola.Points.Blueprint.DictPointBlueprintLibrary.Remove.Failure", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FDictPointBlueprintLibrary_Remove_FailureTest::RunTest(const FString& Parameters)
{
    TInstancedStruct<FDictPoint> DictPoint;
    DictPoint.InitializeAs<FDictPoint>();

    bool Result = UDictPointBlueprintLibrary::DictPoint_Remove(DictPoint, TEXT("nonexistent"));

    TestFalse(TEXT("Remove returned false"), Result);

    return true;
}

// DictPoint_Length Tests

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDictPointBlueprintLibrary_Length_EmptyTest, "Schola.Points.Blueprint.DictPointBlueprintLibrary.Length.Empty", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FDictPointBlueprintLibrary_Length_EmptyTest::RunTest(const FString& Parameters)
{
    TInstancedStruct<FDictPoint> DictPoint;
    DictPoint.InitializeAs<FDictPoint>();

    int32 Result = UDictPointBlueprintLibrary::DictPoint_Length(DictPoint);

    TestEqual(TEXT("Length == 0"), Result, 0);

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDictPointBlueprintLibrary_Length_MultipleTest, "Schola.Points.Blueprint.DictPointBlueprintLibrary.Length.Multiple", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FDictPointBlueprintLibrary_Length_MultipleTest::RunTest(const FString& Parameters)
{
    TInstancedStruct<FDictPoint> DictPoint;
    DictPoint.InitializeAs<FDictPoint>();

    TInstancedStruct<FDiscretePoint> DiscretePoint1 = UDiscretePointBlueprintLibrary::Int32ToDiscretePoint(5);
    TInstancedStruct<FDiscretePoint> DiscretePoint2 = UDiscretePointBlueprintLibrary::Int32ToDiscretePoint(10);
    TInstancedStruct<FDiscretePoint> DiscretePoint3 = UDiscretePointBlueprintLibrary::Int32ToDiscretePoint(15);
    
    UDictPointBlueprintLibrary::DictPoint_Add(DictPoint, TEXT("key1"), ToUntypedInstancedStruct(DiscretePoint1));
	UDictPointBlueprintLibrary::DictPoint_Add(DictPoint, TEXT("key2"), ToUntypedInstancedStruct(DiscretePoint2));
    UDictPointBlueprintLibrary::DictPoint_Add(DictPoint, TEXT("key3"), ToUntypedInstancedStruct(DiscretePoint3));

    int32 Result = UDictPointBlueprintLibrary::DictPoint_Length(DictPoint);

    TestEqual(TEXT("Length == 3"), Result, 3);

    return true;
}

// DictPoint_Clear Tests

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDictPointBlueprintLibrary_Clear_BasicTest, "Schola.Points.Blueprint.DictPointBlueprintLibrary.Clear.Basic", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FDictPointBlueprintLibrary_Clear_BasicTest::RunTest(const FString& Parameters)
{
    TInstancedStruct<FDictPoint> DictPoint;
    DictPoint.InitializeAs<FDictPoint>();

    TInstancedStruct<FDiscretePoint> DiscretePoint1 = UDiscretePointBlueprintLibrary::Int32ToDiscretePoint(5);
    TInstancedStruct<FDiscretePoint> DiscretePoint2 = UDiscretePointBlueprintLibrary::Int32ToDiscretePoint(10);
    
    UDictPointBlueprintLibrary::DictPoint_Add(DictPoint, TEXT("key1"), ToUntypedInstancedStruct(DiscretePoint1));
	UDictPointBlueprintLibrary::DictPoint_Add(DictPoint, TEXT("key2"), ToUntypedInstancedStruct(DiscretePoint2));

    UDictPointBlueprintLibrary::DictPoint_Clear(DictPoint);

    TestEqual(TEXT("DictPoint is now empty"), DictPoint.Get<FDictPoint>().Points.Num(), 0);

    return true;
}

// DictPoint_Keys Tests

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDictPointBlueprintLibrary_Keys_BasicTest, "Schola.Points.Blueprint.DictPointBlueprintLibrary.Keys.Basic", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FDictPointBlueprintLibrary_Keys_BasicTest::RunTest(const FString& Parameters)
{
    TInstancedStruct<FDictPoint> DictPoint;
    DictPoint.InitializeAs<FDictPoint>();

    TInstancedStruct<FDiscretePoint> DiscretePoint1 = UDiscretePointBlueprintLibrary::Int32ToDiscretePoint(5);
    TInstancedStruct<FDiscretePoint> DiscretePoint2 = UDiscretePointBlueprintLibrary::Int32ToDiscretePoint(10);
    
    UDictPointBlueprintLibrary::DictPoint_Add(DictPoint, TEXT("action"), ToUntypedInstancedStruct(DiscretePoint1));
    UDictPointBlueprintLibrary::DictPoint_Add(DictPoint, TEXT("observation"), ToUntypedInstancedStruct(DiscretePoint2));

    TArray<FString> Keys;
    UDictPointBlueprintLibrary::DictPoint_Keys(DictPoint, Keys);

    TestEqual(TEXT("Keys.Num() == 2"), Keys.Num(), 2);
    TestTrue(TEXT("Keys contains 'action'"), Keys.Contains(TEXT("action")));
    TestTrue(TEXT("Keys contains 'observation'"), Keys.Contains(TEXT("observation")));

    return true;
}

// DictPoint_Values Tests

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDictPointBlueprintLibrary_Values_BasicTest, "Schola.Points.Blueprint.DictPointBlueprintLibrary.Values.Basic", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FDictPointBlueprintLibrary_Values_BasicTest::RunTest(const FString& Parameters)
{
    TInstancedStruct<FDictPoint> DictPoint;
    DictPoint.InitializeAs<FDictPoint>();

    TInstancedStruct<FDiscretePoint> DiscretePoint1 = UDiscretePointBlueprintLibrary::Int32ToDiscretePoint(5);
    TInstancedStruct<FDiscretePoint> DiscretePoint2 = UDiscretePointBlueprintLibrary::Int32ToDiscretePoint(10);
    
    UDictPointBlueprintLibrary::DictPoint_Add(DictPoint, TEXT("action"), ToUntypedInstancedStruct(DiscretePoint1));
    UDictPointBlueprintLibrary::DictPoint_Add(DictPoint, TEXT("observation"), ToUntypedInstancedStruct(DiscretePoint2));

    TArray<FInstancedStruct> Values;
    UDictPointBlueprintLibrary::DictPoint_Values(DictPoint, Values);

    TestEqual(TEXT("Values.Num() == 2"), Values.Num(), 2);
    TestTrue(TEXT("Values[0] is valid"), Values[0].IsValid());
    TestTrue(TEXT("Values[1] is valid"), Values[1].IsValid());

    return true;
}

#endif




