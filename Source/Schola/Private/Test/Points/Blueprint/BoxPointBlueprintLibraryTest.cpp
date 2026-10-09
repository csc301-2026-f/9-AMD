// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "Points/Blueprint/BoxPointBlueprintLibrary.h"
#include "Points/BoxPoint.h"

#if WITH_DEV_AUTOMATION_TESTS

#define TestEqualExactFloat(TestMessage, Actual, Expected) TestEqual(TestMessage, (float)Actual, (float)Expected, 0.0001f)

// ArrayToBoxPoint Tests

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBoxPointBlueprintLibrary_ArrayToBoxPoint_BasicTest, "Schola.Points.Blueprint.BoxPointBlueprintLibrary.ArrayToBoxPoint.Basic", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBoxPointBlueprintLibrary_ArrayToBoxPoint_BasicTest::RunTest(const FString& Parameters)
{
    TArray<float> Values = {1.0f, 2.0f, 3.0f};

    TInstancedStruct<FBoxPoint> Result = UBoxPointBlueprintLibrary::ArrayToBoxPoint(Values);

    TestTrue(TEXT("Result is valid"), Result.IsValid());
    
    const FBoxPoint& BoxPoint = Result.Get<FBoxPoint>();
    TestEqual(TEXT("BoxPoint.Values"), BoxPoint.Values, Values);

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBoxPointBlueprintLibrary_ArrayToBoxPoint_EmptyTest, "Schola.Points.Blueprint.BoxPointBlueprintLibrary.ArrayToBoxPoint.Empty", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBoxPointBlueprintLibrary_ArrayToBoxPoint_EmptyTest::RunTest(const FString& Parameters)
{
    TArray<float> Values;

    TInstancedStruct<FBoxPoint> Result = UBoxPointBlueprintLibrary::ArrayToBoxPoint(Values);

    TestTrue(TEXT("Result is valid"), Result.IsValid());
    
    const FBoxPoint& BoxPoint = Result.Get<FBoxPoint>();
    TestEqual(TEXT("BoxPoint.Values.Num() == 0"), BoxPoint.Values.Num(), 0);

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBoxPointBlueprintLibrary_ArrayToBoxPoint_NegativeTest, "Schola.Points.Blueprint.BoxPointBlueprintLibrary.ArrayToBoxPoint.Negative", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBoxPointBlueprintLibrary_ArrayToBoxPoint_NegativeTest::RunTest(const FString& Parameters)
{
    TArray<float> Values = {-5.0f, -10.0f, -15.0f};

    TInstancedStruct<FBoxPoint> Result = UBoxPointBlueprintLibrary::ArrayToBoxPoint(Values);

    TestTrue(TEXT("Result is valid"), Result.IsValid());
    
    const FBoxPoint& BoxPoint = Result.Get<FBoxPoint>();
    TestEqual(TEXT("BoxPoint.Values"), BoxPoint.Values, Values);

    return true;
}

// ArrayToBoxPointShaped Tests

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBoxPointBlueprintLibrary_ArrayToBoxPointShaped_BasicTest, "Schola.Points.Blueprint.BoxPointBlueprintLibrary.ArrayToBoxPointShaped.Basic", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBoxPointBlueprintLibrary_ArrayToBoxPointShaped_BasicTest::RunTest(const FString& Parameters)
{
    TArray<float> Values = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f};
    TArray<int32> Shape = {2, 3};

    TInstancedStruct<FBoxPoint> Result = UBoxPointBlueprintLibrary::ArrayToBoxPointShaped(Values, Shape);

    TestTrue(TEXT("Result is valid"), Result.IsValid());
    
    const FBoxPoint& BoxPoint = Result.Get<FBoxPoint>();
    TestEqual(TEXT("BoxPoint.Values"), BoxPoint.Values, Values);
    TestEqual(TEXT("BoxPoint.Shape"), BoxPoint.Shape, Shape);

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBoxPointBlueprintLibrary_ArrayToBoxPointShaped_3DTest, "Schola.Points.Blueprint.BoxPointBlueprintLibrary.ArrayToBoxPointShaped.3D", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBoxPointBlueprintLibrary_ArrayToBoxPointShaped_3DTest::RunTest(const FString& Parameters)
{
    TArray<float> Values;
    for (int32 i = 0; i < 24; i++)
    {
        Values.Add(static_cast<float>(i));
    }
    TArray<int32> Shape = {2, 3, 4};

    TInstancedStruct<FBoxPoint> Result = UBoxPointBlueprintLibrary::ArrayToBoxPointShaped(Values, Shape);

    TestTrue(TEXT("Result is valid"), Result.IsValid());
    
    const FBoxPoint& BoxPoint = Result.Get<FBoxPoint>();
    TestEqual(TEXT("BoxPoint.Values"), BoxPoint.Values, Values);
    TestEqual(TEXT("BoxPoint.Shape"), BoxPoint.Shape, Shape);

    return true;
}

// BoxPointToArray Tests

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBoxPointBlueprintLibrary_BoxPointToArray_BasicTest, "Schola.Points.Blueprint.BoxPointBlueprintLibrary.BoxPointToArray.Basic", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBoxPointBlueprintLibrary_BoxPointToArray_BasicTest::RunTest(const FString& Parameters)
{
    TInstancedStruct<FBoxPoint> Point;
    Point.InitializeAs<FBoxPoint>();
    FBoxPoint& BoxPoint = Point.GetMutable<FBoxPoint>();
    BoxPoint.Values = {7.5f, 8.5f, 9.5f};

    TArray<float> Result = UBoxPointBlueprintLibrary::BoxPointToArray(Point);

    TestEqual(TEXT("Result"), Result, TArray<float>({7.5f, 8.5f, 9.5f}));

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBoxPointBlueprintLibrary_BoxPointToArray_RoundTripTest, "Schola.Points.Blueprint.BoxPointBlueprintLibrary.BoxPointToArray.RoundTrip", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBoxPointBlueprintLibrary_BoxPointToArray_RoundTripTest::RunTest(const FString& Parameters)
{
    TArray<float> OriginalValues = {1.1f, 2.2f, 3.3f, 4.4f};

    TInstancedStruct<FBoxPoint> Point = UBoxPointBlueprintLibrary::ArrayToBoxPoint(OriginalValues);
    TArray<float> Result = UBoxPointBlueprintLibrary::BoxPointToArray(Point);

    TestEqual(TEXT("Round trip array length"), Result.Num(), OriginalValues.Num());
    for (int32 i = 0; i < OriginalValues.Num(); i++)
    {
        TestEqualExactFloat(FString::Printf(TEXT("Round trip array[%d]"), i), Result[i], OriginalValues[i]);
    }

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBoxPointBlueprintLibrary_BoxPointToArray_ShapedRoundTripTest, "Schola.Points.Blueprint.BoxPointBlueprintLibrary.BoxPointToArray.ShapedRoundTrip", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBoxPointBlueprintLibrary_BoxPointToArray_ShapedRoundTripTest::RunTest(const FString& Parameters)
{
    TArray<float> OriginalValues = {1.0f, 2.0f, 3.0f, 4.0f};
    TArray<int32> OriginalShape = {2, 2};

    TInstancedStruct<FBoxPoint> Point = UBoxPointBlueprintLibrary::ArrayToBoxPointShaped(OriginalValues, OriginalShape);
    TArray<float> Result = UBoxPointBlueprintLibrary::BoxPointToArray(Point);

    TestEqual(TEXT("Round trip array"), Result, OriginalValues);

    return true;
}

// VectorToBoxPoint Tests

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBoxPointBlueprintLibrary_VectorToBoxPoint_BasicTest, "Schola.Points.Blueprint.BoxPointBlueprintLibrary.VectorToBoxPoint.Basic", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBoxPointBlueprintLibrary_VectorToBoxPoint_BasicTest::RunTest(const FString& Parameters)
{
    FVector TestVector(10.0f, 20.0f, 30.0f);

    TInstancedStruct<FBoxPoint> Result = UBoxPointBlueprintLibrary::VectorToBoxPoint(TestVector);

    TestTrue(TEXT("Result is valid"), Result.IsValid());
    
    const FBoxPoint& BoxPoint = Result.Get<FBoxPoint>();
    TestEqual(TEXT("BoxPoint.Values"), BoxPoint.Values, TArray<float>({10.0f, 20.0f, 30.0f}));

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBoxPointBlueprintLibrary_VectorToBoxPoint_ZeroVectorTest, "Schola.Points.Blueprint.BoxPointBlueprintLibrary.VectorToBoxPoint.ZeroVector", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBoxPointBlueprintLibrary_VectorToBoxPoint_ZeroVectorTest::RunTest(const FString& Parameters)
{
    FVector TestVector = FVector::ZeroVector;

    TInstancedStruct<FBoxPoint> Result = UBoxPointBlueprintLibrary::VectorToBoxPoint(TestVector);

    TestTrue(TEXT("Result is valid"), Result.IsValid());
    
    const FBoxPoint& BoxPoint = Result.Get<FBoxPoint>();
    TestEqual(TEXT("BoxPoint.Values"), BoxPoint.Values, TArray<float>({0.0f, 0.0f, 0.0f}));

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBoxPointBlueprintLibrary_VectorToBoxPoint_NegativeTest, "Schola.Points.Blueprint.BoxPointBlueprintLibrary.VectorToBoxPoint.Negative", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBoxPointBlueprintLibrary_VectorToBoxPoint_NegativeTest::RunTest(const FString& Parameters)
{
    FVector TestVector(-5.5f, -10.5f, -15.5f);

    TInstancedStruct<FBoxPoint> Result = UBoxPointBlueprintLibrary::VectorToBoxPoint(TestVector);

    TestTrue(TEXT("Result is valid"), Result.IsValid());
    
    const FBoxPoint& BoxPoint = Result.Get<FBoxPoint>();
    TestEqual(TEXT("BoxPoint.Values"), BoxPoint.Values, TArray<float>({-5.5f, -10.5f, -15.5f}));

    return true;
}

// BoxPointToVector Tests

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBoxPointBlueprintLibrary_BoxPointToVector_BasicTest, "Schola.Points.Blueprint.BoxPointBlueprintLibrary.BoxPointToVector.Basic", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBoxPointBlueprintLibrary_BoxPointToVector_BasicTest::RunTest(const FString& Parameters)
{
    TArray<float> Values = {100.0f, 200.0f, 300.0f};
    TInstancedStruct<FBoxPoint> Point = UBoxPointBlueprintLibrary::ArrayToBoxPoint(Values);

    FVector Result = UBoxPointBlueprintLibrary::BoxPointToVector(Point);

    TestEqualExactFloat(TEXT("Result.X == 100.0"), Result.X, 100.0f);
    TestEqualExactFloat(TEXT("Result.Y == 200.0"), Result.Y, 200.0f);
    TestEqualExactFloat(TEXT("Result.Z == 300.0"), Result.Z, 300.0f);

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBoxPointBlueprintLibrary_BoxPointToVector_RoundTripTest, "Schola.Points.Blueprint.BoxPointBlueprintLibrary.BoxPointToVector.RoundTrip", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBoxPointBlueprintLibrary_BoxPointToVector_RoundTripTest::RunTest(const FString& Parameters)
{
    FVector OriginalVector(42.5f, 73.2f, -15.8f);

    TInstancedStruct<FBoxPoint> Point = UBoxPointBlueprintLibrary::VectorToBoxPoint(OriginalVector);
    FVector Result = UBoxPointBlueprintLibrary::BoxPointToVector(Point);

    TestEqualExactFloat(TEXT("Round trip X"), Result.X, OriginalVector.X);
    TestEqualExactFloat(TEXT("Round trip Y"), Result.Y, OriginalVector.Y);
    TestEqualExactFloat(TEXT("Round trip Z"), Result.Z, OriginalVector.Z);

    return true;
}

// RotatorToBoxPoint Tests

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBoxPointBlueprintLibrary_RotatorToBoxPoint_BasicTest, "Schola.Points.Blueprint.BoxPointBlueprintLibrary.RotatorToBoxPoint.Basic", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBoxPointBlueprintLibrary_RotatorToBoxPoint_BasicTest::RunTest(const FString& Parameters)
{
    FRotator TestRotator(45.0f, 90.0f, 180.0f);
    // RotatorToBoxPoint normalizes the rotator to [-180, 180] range
    FRotator NormalizedRotator = TestRotator.GetNormalized();

    TInstancedStruct<FBoxPoint> Result = UBoxPointBlueprintLibrary::RotatorToBoxPoint(TestRotator);

    TestTrue(TEXT("Result is valid"), Result.IsValid());
    
    const FBoxPoint& BoxPoint = Result.Get<FBoxPoint>();
    const TArray<float> ExpectedValues = {
        static_cast<float>(NormalizedRotator.Pitch),
        static_cast<float>(NormalizedRotator.Yaw),
        static_cast<float>(NormalizedRotator.Roll),
    };
    TestEqual(TEXT("BoxPoint.Values"), BoxPoint.Values, ExpectedValues);

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBoxPointBlueprintLibrary_RotatorToBoxPoint_ZeroRotatorTest, "Schola.Points.Blueprint.BoxPointBlueprintLibrary.RotatorToBoxPoint.ZeroRotator", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBoxPointBlueprintLibrary_RotatorToBoxPoint_ZeroRotatorTest::RunTest(const FString& Parameters)
{
    FRotator TestRotator = FRotator::ZeroRotator;

    TInstancedStruct<FBoxPoint> Result = UBoxPointBlueprintLibrary::RotatorToBoxPoint(TestRotator);

    TestTrue(TEXT("Result is valid"), Result.IsValid());
    
    const FBoxPoint& BoxPoint = Result.Get<FBoxPoint>();
    TestEqual(TEXT("BoxPoint.Values"), BoxPoint.Values, TArray<float>({0.0f, 0.0f, 0.0f}));

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBoxPointBlueprintLibrary_RotatorToBoxPoint_NegativeAnglesTest, "Schola.Points.Blueprint.BoxPointBlueprintLibrary.RotatorToBoxPoint.NegativeAngles", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBoxPointBlueprintLibrary_RotatorToBoxPoint_NegativeAnglesTest::RunTest(const FString& Parameters)
{
    FRotator TestRotator(-30.0f, -60.0f, -90.0f);

    TInstancedStruct<FBoxPoint> Result = UBoxPointBlueprintLibrary::RotatorToBoxPoint(TestRotator);

    TestTrue(TEXT("Result is valid"), Result.IsValid());
    
    const FBoxPoint& BoxPoint = Result.Get<FBoxPoint>();
    TestEqual(TEXT("BoxPoint.Values"), BoxPoint.Values, TArray<float>({-30.0f, -60.0f, -90.0f}));

    return true;
}

// BoxPointToRotator Tests

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBoxPointBlueprintLibrary_BoxPointToRotator_BasicTest, "Schola.Points.Blueprint.BoxPointBlueprintLibrary.BoxPointToRotator.Basic", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBoxPointBlueprintLibrary_BoxPointToRotator_BasicTest::RunTest(const FString& Parameters)
{
    TArray<float> Values = {30.0f, 60.0f, 90.0f};
    TInstancedStruct<FBoxPoint> Point = UBoxPointBlueprintLibrary::ArrayToBoxPoint(Values);

    FRotator Result = UBoxPointBlueprintLibrary::BoxPointToRotator(Point);

    TestEqualExactFloat(TEXT("Result.Pitch == 30.0"), Result.Pitch, 30.0f);
    TestEqualExactFloat(TEXT("Result.Yaw == 60.0"), Result.Yaw, 60.0f);
    TestEqualExactFloat(TEXT("Result.Roll == 90.0"), Result.Roll, 90.0f);

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBoxPointBlueprintLibrary_BoxPointToRotator_RoundTripTest, "Schola.Points.Blueprint.BoxPointBlueprintLibrary.BoxPointToRotator.RoundTrip", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBoxPointBlueprintLibrary_BoxPointToRotator_RoundTripTest::RunTest(const FString& Parameters)
{
    FRotator OriginalRotator(22.5f, 67.3f, -45.8f);
    // RotatorToBoxPoint normalizes the rotator to [-180, 180] range
    FRotator NormalizedRotator = OriginalRotator.GetNormalized();

    TInstancedStruct<FBoxPoint> Point = UBoxPointBlueprintLibrary::RotatorToBoxPoint(OriginalRotator);
    FRotator Result = UBoxPointBlueprintLibrary::BoxPointToRotator(Point);

    TestEqualExactFloat(TEXT("Round trip Pitch"), Result.Pitch, NormalizedRotator.Pitch);
    TestEqualExactFloat(TEXT("Round trip Yaw"), Result.Yaw, NormalizedRotator.Yaw);
    TestEqualExactFloat(TEXT("Round trip Roll"), Result.Roll, NormalizedRotator.Roll);

    return true;
}

// TransformToBoxPoint Tests

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBoxPointBlueprintLibrary_TransformToBoxPoint_BasicTest, "Schola.Points.Blueprint.BoxPointBlueprintLibrary.TransformToBoxPoint.Basic", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBoxPointBlueprintLibrary_TransformToBoxPoint_BasicTest::RunTest(const FString& Parameters)
{
    FVector Location(100.0f, 200.0f, 300.0f);
    FRotator Rotation(45.0f, 90.0f, 135.0f);
    FVector Scale(1.5f, 2.0f, 2.5f);
    FTransform TestTransform(Rotation, Location, Scale);

    TInstancedStruct<FBoxPoint> Result = UBoxPointBlueprintLibrary::TransformToBoxPoint(TestTransform);

    TestTrue(TEXT("Result is valid"), Result.IsValid());
    
    const FBoxPoint& BoxPoint = Result.Get<FBoxPoint>();
    TestEqual(TEXT("BoxPoint.Values.Num() == 9"), BoxPoint.Values.Num(), 9);
    
    // Location
    TestEqualExactFloat(TEXT("BoxPoint[0] == 100.0 (Location.X)"), BoxPoint[0], 100.0f);
    TestEqualExactFloat(TEXT("BoxPoint[1] == 200.0 (Location.Y)"), BoxPoint[1], 200.0f);
    TestEqualExactFloat(TEXT("BoxPoint[2] == 300.0 (Location.Z)"), BoxPoint[2], 300.0f);
    
    // Rotation
    TestEqualExactFloat(TEXT("BoxPoint[3] == 45.0 (Rotation.Pitch)"), BoxPoint[3], 45.0f);
    TestEqualExactFloat(TEXT("BoxPoint[4] == 90.0 (Rotation.Yaw)"), BoxPoint[4], 90.0f);
    TestEqualExactFloat(TEXT("BoxPoint[5] == 135.0 (Rotation.Roll)"), BoxPoint[5], 135.0f);
    
    // Scale
    TestEqualExactFloat(TEXT("BoxPoint[6] == 1.5 (Scale.X)"), BoxPoint[6], 1.5f);
    TestEqualExactFloat(TEXT("BoxPoint[7] == 2.0 (Scale.Y)"), BoxPoint[7], 2.0f);
    TestEqualExactFloat(TEXT("BoxPoint[8] == 2.5 (Scale.Z)"), BoxPoint[8], 2.5f);

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBoxPointBlueprintLibrary_TransformToBoxPoint_IdentityTest, "Schola.Points.Blueprint.BoxPointBlueprintLibrary.TransformToBoxPoint.Identity", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBoxPointBlueprintLibrary_TransformToBoxPoint_IdentityTest::RunTest(const FString& Parameters)
{
    FTransform TestTransform = FTransform::Identity;

    TInstancedStruct<FBoxPoint> Result = UBoxPointBlueprintLibrary::TransformToBoxPoint(TestTransform);

    TestTrue(TEXT("Result is valid"), Result.IsValid());
    
    const FBoxPoint& BoxPoint = Result.Get<FBoxPoint>();
    TestEqual(TEXT("BoxPoint.Values.Num() == 9"), BoxPoint.Values.Num(), 9);
    
    // Location should be zero
    TestEqualExactFloat(TEXT("BoxPoint[0] == 0.0"), BoxPoint[0], 0.0f);
    TestEqualExactFloat(TEXT("BoxPoint[1] == 0.0"), BoxPoint[1], 0.0f);
    TestEqualExactFloat(TEXT("BoxPoint[2] == 0.0"), BoxPoint[2], 0.0f);
    
    // Rotation should be zero
    TestEqualExactFloat(TEXT("BoxPoint[3] == 0.0"), BoxPoint[3], 0.0f);
    TestEqualExactFloat(TEXT("BoxPoint[4] == 0.0"), BoxPoint[4], 0.0f);
    TestEqualExactFloat(TEXT("BoxPoint[5] == 0.0"), BoxPoint[5], 0.0f);
    
    // Scale should be one
    TestEqualExactFloat(TEXT("BoxPoint[6] == 1.0"), BoxPoint[6], 1.0f);
    TestEqualExactFloat(TEXT("BoxPoint[7] == 1.0"), BoxPoint[7], 1.0f);
    TestEqualExactFloat(TEXT("BoxPoint[8] == 1.0"), BoxPoint[8], 1.0f);

    return true;
}

// BoxPointToTransform Tests

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBoxPointBlueprintLibrary_BoxPointToTransform_BasicTest, "Schola.Points.Blueprint.BoxPointBlueprintLibrary.BoxPointToTransform.Basic", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBoxPointBlueprintLibrary_BoxPointToTransform_BasicTest::RunTest(const FString& Parameters)
{
    TArray<float> Values = {50.0f, 100.0f, 150.0f, 30.0f, 60.0f, 90.0f, 0.5f, 1.0f, 1.5f};
    TInstancedStruct<FBoxPoint> Point = UBoxPointBlueprintLibrary::ArrayToBoxPoint(Values);

    FTransform Result = UBoxPointBlueprintLibrary::BoxPointToTransform(Point);

    // Location
    TestEqualExactFloat(TEXT("Result.Location.X == 50.0"), Result.GetLocation().X, 50.0f);
    TestEqualExactFloat(TEXT("Result.Location.Y == 100.0"), Result.GetLocation().Y, 100.0f);
    TestEqualExactFloat(TEXT("Result.Location.Z == 150.0"), Result.GetLocation().Z, 150.0f);
    
    // Rotation
    FRotator ResultRotation = Result.Rotator();
    TestEqualExactFloat(TEXT("Result.Rotation.Pitch == 30.0"), ResultRotation.Pitch, 30.0f);
    TestEqualExactFloat(TEXT("Result.Rotation.Yaw == 60.0"), ResultRotation.Yaw, 60.0f);
    TestEqualExactFloat(TEXT("Result.Rotation.Roll == 90.0"), ResultRotation.Roll, 90.0f);
    
    // Scale
    TestEqualExactFloat(TEXT("Result.Scale.X == 0.5"), Result.GetScale3D().X, 0.5f);
    TestEqualExactFloat(TEXT("Result.Scale.Y == 1.0"), Result.GetScale3D().Y, 1.0f);
    TestEqualExactFloat(TEXT("Result.Scale.Z == 1.5"), Result.GetScale3D().Z, 1.5f);

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBoxPointBlueprintLibrary_BoxPointToTransform_RoundTripTest, "Schola.Points.Blueprint.BoxPointBlueprintLibrary.BoxPointToTransform.RoundTrip", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBoxPointBlueprintLibrary_BoxPointToTransform_RoundTripTest::RunTest(const FString& Parameters)
{
    FVector OriginalLocation(123.45f, 234.56f, 345.67f);
    FRotator OriginalRotation(12.34f, 23.45f, 34.56f);
    FVector OriginalScale(0.75f, 1.25f, 1.75f);
    FTransform OriginalTransform(OriginalRotation, OriginalLocation, OriginalScale);

    TInstancedStruct<FBoxPoint> Point = UBoxPointBlueprintLibrary::TransformToBoxPoint(OriginalTransform);
    FTransform Result = UBoxPointBlueprintLibrary::BoxPointToTransform(Point);

    // Location
    TestEqualExactFloat(TEXT("Round trip Location.X"), Result.GetLocation().X, OriginalLocation.X);
    TestEqualExactFloat(TEXT("Round trip Location.Y"), Result.GetLocation().Y, OriginalLocation.Y);
    TestEqualExactFloat(TEXT("Round trip Location.Z"), Result.GetLocation().Z, OriginalLocation.Z);
    
    // Rotation - TransformToBoxPoint stores raw rotation values (not normalized)
    FRotator ResultRotation = Result.Rotator();
    TestEqualExactFloat(TEXT("Round trip Rotation.Pitch"), ResultRotation.Pitch, OriginalRotation.Pitch);
    TestEqualExactFloat(TEXT("Round trip Rotation.Yaw"), ResultRotation.Yaw, OriginalRotation.Yaw);
    TestEqualExactFloat(TEXT("Round trip Rotation.Roll"), ResultRotation.Roll, OriginalRotation.Roll);
    
    // Scale
    TestEqualExactFloat(TEXT("Round trip Scale.X"), Result.GetScale3D().X, OriginalScale.X);
    TestEqualExactFloat(TEXT("Round trip Scale.Y"), Result.GetScale3D().Y, OriginalScale.Y);
    TestEqualExactFloat(TEXT("Round trip Scale.Z"), Result.GetScale3D().Z, OriginalScale.Z);

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBoxPointBlueprintLibrary_BoxPointToTransform_ComplexTest, "Schola.Points.Blueprint.BoxPointBlueprintLibrary.BoxPointToTransform.Complex", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FBoxPointBlueprintLibrary_BoxPointToTransform_ComplexTest::RunTest(const FString& Parameters)
{
    // Test with negative values and various scales
    FVector Location(-100.0f, 500.0f, -250.0f);
    FRotator Rotation(-45.0f, 270.0f, -135.0f);
    FVector Scale(2.5f, 0.1f, 10.0f);
    FTransform OriginalTransform(Rotation, Location, Scale);

    TInstancedStruct<FBoxPoint> Point = UBoxPointBlueprintLibrary::TransformToBoxPoint(OriginalTransform);
    FTransform Result = UBoxPointBlueprintLibrary::BoxPointToTransform(Point);

    // Location
    TestEqualExactFloat(TEXT("Complex Location.X"), Result.GetLocation().X, Location.X);
    TestEqualExactFloat(TEXT("Complex Location.Y"), Result.GetLocation().Y, Location.Y);
    TestEqualExactFloat(TEXT("Complex Location.Z"), Result.GetLocation().Z, Location.Z);
    
    // Rotation - TransformToBoxPoint stores raw rotation values (Normalized)
    FRotator ResultRotation = Result.Rotator();
    TestEqualExactFloat(TEXT("Complex Rotation.Pitch"), ResultRotation.Pitch, Rotation.Pitch);
    TestEqualExactFloat(TEXT("Complex Rotation.Yaw"), ResultRotation.Yaw, -90.0f);
    TestEqualExactFloat(TEXT("Complex Rotation.Roll"), ResultRotation.Roll, Rotation.Roll);
    
    // Scale
    TestEqualExactFloat(TEXT("Complex Scale.X"), Result.GetScale3D().X, Scale.X);
    TestEqualExactFloat(TEXT("Complex Scale.Y"), Result.GetScale3D().Y, Scale.Y);
    TestEqualExactFloat(TEXT("Complex Scale.Z"), Result.GetScale3D().Z, Scale.Z);

    return true;
}

#endif




