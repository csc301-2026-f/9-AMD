// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "Adapters/BoxStacker.h"
#include "Points/BoxPoint.h"
#include "Points/DiscretePoint.h"
#include "Spaces/BoxSpace.h"
#include "Points/Blueprint/BoxPointBlueprintLibrary.h"
#include "Spaces/Blueprint/BoxSpaceBlueprintLibrary.h"
#include "Common/InstancedStructUtils.h"

#if WITH_DEV_AUTOMATION_TESTS

int32 BXTestGetProduct(TConstArrayView<int> Shape)
{
	if (Shape.Num() == 0)
	{
		return 0;
	}
	int32 Product = 1;
	for (int32 Dim : Shape)
	{
		Product *= Dim;
	}
	return Product;
}

static UBoxStacker* CreateBoxStacker(int32 StackSize = 4, bool bAddNewDimension = true, int32 StackDimensionIndex = 0)
{
	UBoxStacker* Stacker = NewObject<UBoxStacker>();
	if (Stacker)
	{
		Stacker->StackSize = StackSize;
		Stacker->bAddNewDimension = bAddNewDimension;
		Stacker->StackDimensionIndex = StackDimensionIndex;
	}
	return Stacker;
}

static TInstancedStruct<FBoxPoint> MakeBoxPoint(float Value, int32 NumElements)
{
	TArray<float> Values;
	for (int32 i = 0; i < NumElements; ++i)
	{
		Values.Add(Value);
	}
	return UBoxPointBlueprintLibrary::ArrayToBoxPoint(Values);
}

static TInstancedStruct<FBoxPoint> MakeBoxPointShaped(const TArray<float>& Values, const TArray<int>& Shape)
{
	return UBoxPointBlueprintLibrary::ArrayToBoxPointShaped(Values, Shape);
}

static void TestShape(FAutomationTestBase& Test, const TArray<int>& Shape, const TArray<int>& ExpectedShape)
{
	Test.TestEqual(TEXT("Shape Num Dimensions"), Shape.Num(), ExpectedShape.Num());
	if (Shape.Num() == ExpectedShape.Num())
	{
		for (int32 i = 0; i < Shape.Num(); ++i)
		{
			Test.TestEqual(TEXT("Shape Dimension " + FString::FromInt(i)), Shape[i], ExpectedShape[i]);
		}
	}
}

static void TestShape(FAutomationTestBase& Test, const FBoxPoint& Stacked, const TArray<int>& ExpectedShape)
{
	TestShape(Test, Stacked.Shape, ExpectedShape);
}

static void SetupStacker(UBoxStacker* Stacker, const TArray<int>& Shape, float LowValue = 0.0f, float HighValue = 1.0f)
{
	TArray<float> Low, High, Values;
	int			  TotalSize = BXTestGetProduct(Shape);
	
	Low.Init(LowValue, TotalSize);
	High.Init(HighValue, TotalSize);
	Stacker->UnstackedSpace = UBoxSpaceBlueprintLibrary::ArraysToBoxSpace(Low, High, Shape);

	// Picks middle point of each dim, default is 0.5f
	Values.Init(LowValue + (HighValue - LowValue) / 2, BXTestGetProduct(Shape));

	TInstancedStruct<FBoxPoint> DefaultPoint = MakeBoxPointShaped(Values, Shape);
	Stacker->DefaultPoint = reinterpret_cast<const TInstancedStruct<FPoint>&>(DefaultPoint);
}

static void SetupStackerWithNonUniformSpace(UBoxStacker* Stacker, const TArray<int>& Shape)
{
	TArray<float> Low, High, Values;
	for (int32 i = 0; i < BXTestGetProduct(Shape); ++i)
	{
		Low.Add(static_cast<float>(-(i+1)));
		High.Add(static_cast<float>(i+1));
		Values.Add(0.0f);
	}

	Stacker->UnstackedSpace = UBoxSpaceBlueprintLibrary::ArraysToBoxSpace(Low, High, Shape);
	TInstancedStruct<FBoxPoint> DefaultPoint = MakeBoxPointShaped(Values, Shape);
	Stacker->DefaultPoint = reinterpret_cast<const TInstancedStruct<FPoint>&>(DefaultPoint);
}

// Each point counts up from 0 to GetTotalSize(Shape) - 1, then we multiple to add a frame identifier
static void FillStacker(UBoxStacker* Stacker, const TArray<int>& Shape)
{

	TArray<float> Values;
	Values.Init(0.0f, BXTestGetProduct(Shape));

	for (int32 frame = 0; frame < Stacker->StackSize; ++frame)
	{
		for (int32 i = 0; i < BXTestGetProduct(Shape); ++i)
		{
			Values[i] = static_cast<float>(frame * BXTestGetProduct(Shape) + i);
		}
		TInstancedStruct<FBoxPoint> InPoint = MakeBoxPointShaped(Values, Shape);
		FInstancedStruct InPointInstanced = ToUntypedInstancedStruct(InPoint);
		FInstancedStruct OutStacked;
		Stacker->Push(InPointInstanced, OutStacked);
	}
}


// Push single entry, GetNumValid == 1
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBoxStacker_PushSingle_NumValidOne, "Schola.Adapters.BoxStacker.Push.Push Single Value", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FBoxStacker_PushSingle_NumValidOne::RunTest(const FString& Parameters)
{
	UBoxStacker* Stacker = CreateBoxStacker(4);
	if (!TestNotNull(TEXT("Stacker created"), Stacker)) return false;
	SetupStacker(Stacker, { 10 });
	TInstancedStruct<FBoxPoint> InPoint = MakeBoxPoint(1.0f, 10);
	FInstancedStruct InPointInstanced = ToUntypedInstancedStruct(InPoint);
	FInstancedStruct OutStacked;
	Stacker->Push(InPointInstanced, OutStacked);

	TestEqual(TEXT("GetNumValid() == 1"), Stacker->GetNumValid(), 1);
	return true;
}

// Push non-BoxPoint: logs error and does not add to stack
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBoxStacker_Push_NonBoxPoint_LogsError, "Schola.Adapters.BoxStacker.Push.Non BoxPoint Logs Error", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FBoxStacker_Push_NonBoxPoint_LogsError::RunTest(const FString& Parameters)
{
	UBoxStacker* Stacker = CreateBoxStacker(4);
	if (!TestNotNull(TEXT("Stacker created"), Stacker)) return false;
	SetupStacker(Stacker, { 1, 10 }, 0.0f, 10.0f);

	// FInstancedStruct holding FDiscretePoint - not a FBoxPoint
	FInstancedStruct NonBoxPoint;
	NonBoxPoint.InitializeAs<FDiscretePoint>(0);

	AddExpectedError(TEXT("Box Stacker only supports FBoxPoint"), EAutomationExpectedErrorFlags::Contains, 1);
	FInstancedStruct OutStacked;
	Stacker->Push(NonBoxPoint, OutStacked);

	TestEqual(TEXT("GetNumValid() == 0 (no point added)"), Stacker->GetNumValid(), 0);
	TestTrue(TEXT("OutStacked still valid (GetStacked was called)"), OutStacked.IsValid());
	return true;
}

// Push 5 points of shape (1,10), GetStacked has shape (5,1,10) when bAddNewDimension=true
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBoxStacker_Push_NewDim_0, "Schola.Adapters.BoxStacker.Push.Push Add New Dim 0", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FBoxStacker_Push_NewDim_0::RunTest(const FString& Parameters)
{
	UBoxStacker* Stacker = CreateBoxStacker(5, true, 0);
	if (!TestNotNull(TEXT("Stacker created"), Stacker)) return false;
    
	
	TArray<int> Shape = { 1, 10 };
	SetupStacker(Stacker, Shape, 0.0f, 50.0f);
	FillStacker(Stacker, Shape);

	FBoxPoint Stacked;
	Stacker->GetStacked(Stacked);

	TestShape(*this, Stacked, {5, 1, 10});
	// Memory layout: etc.
	// Frame 4 (newest) = last pushed: values 40..49
	TestEqual(TEXT("Stacked[0] = frame 4 [0,0]"), Stacked.Values[0], 40.0f);
	TestEqual(TEXT("Stacked[1] = frame 4 [0,1]"), Stacked.Values[1], 41.0f);
	// Frame 3: values 30..39
	TestEqual(TEXT("Stacked[10] = frame 3 [0,0]"), Stacked.Values[10], 30.0f);
	TestEqual(TEXT("Stacked[11] = frame 3 [0,1]"), Stacked.Values[11], 31.0f);
	// Frame 2: values 20..29
	TestEqual(TEXT("Stacked[20] = frame 2 [0,0]"), Stacked.Values[20], 20.0f);
	TestEqual(TEXT("Stacked[21] = frame 2 [0,1]"), Stacked.Values[21], 21.0f);
	// Frame 1: values 10..19
	TestEqual(TEXT("Stacked[30] = frame 1 [0,0]"), Stacked.Values[30], 10.0f);
	TestEqual(TEXT("Stacked[31] = frame 1 [0,1]"), Stacked.Values[31], 11.0f);
	// Frame 0: values 0..9
	TestEqual(TEXT("Stacked[40] = frame 0 [0,0]"), Stacked.Values[40], 0.0f);
	TestEqual(TEXT("Stacked[41] = frame 0 [0,1]"), Stacked.Values[41], 1.0f);

	return true;
}

// Push 5 points of shape (1,10), GetStacked has shape (1,5,10) when bAddNewDimension=false
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBoxStacker_Push_ExpandDim_0, "Schola.Adapters.BoxStacker.Push.Push Expand Dim 0", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FBoxStacker_Push_ExpandDim_0::RunTest(const FString& Parameters)
{
	UBoxStacker* Stacker = CreateBoxStacker(5, false, 0);
	if (!TestNotNull(TEXT("Stacker created"), Stacker)) return false;

	TArray<int> Shape = { 1, 10 };
	SetupStacker(Stacker, Shape, 0.0f, 50.0f);
	FillStacker(Stacker, Shape);

	FBoxPoint Stacked;
	Stacker->GetStacked(Stacked);

	TestShape(*this, Stacked, {5, 10});
	TestEqual(TEXT("Values.Num() == 50"), Stacked.Values.Num(), 50);

	// Memory layout: Values[0..9]=frame0, Values[10..19]=frame1, etc.
	TestEqual(TEXT("Stacked[0] = frame 4 [0,0]"), Stacked.Values[0], 40.0f);
	TestEqual(TEXT("Stacked[1] = frame 4 [0,1]"), Stacked.Values[1], 41.0f);
	// Frame 3: values 30..39
	TestEqual(TEXT("Stacked[10] = frame 3 [0,0]"), Stacked.Values[10], 30.0f);
	TestEqual(TEXT("Stacked[11] = frame 3 [0,1]"), Stacked.Values[11], 31.0f);
	// Frame 2: values 20..29
	TestEqual(TEXT("Stacked[20] = frame 2 [0,0]"), Stacked.Values[20], 20.0f);
	TestEqual(TEXT("Stacked[21] = frame 2 [0,1]"), Stacked.Values[21], 21.0f);
	// Frame 1: values 10..19
	TestEqual(TEXT("Stacked[30] = frame 1 [0,0]"), Stacked.Values[30], 10.0f);
	TestEqual(TEXT("Stacked[31] = frame 1 [0,1]"), Stacked.Values[31], 11.0f);
	// Frame 0: values 0..9
	TestEqual(TEXT("Stacked[40] = frame 0 [0,0]"), Stacked.Values[40], 0.0f);
	TestEqual(TEXT("Stacked[41] = frame 0 [0,1]"), Stacked.Values[41], 1.0f);

	return true;
}

// Push 2 points of shape (2,2,2); expand dim 1 gives GetStacked shape (2,4,2) when bAddNewDimension=false
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBoxStacker_PushExpandDim_1, "Schola.Adapters.BoxStacker.Push.Push Expand Dim 1", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FBoxStacker_PushExpandDim_1::RunTest(const FString& Parameters)
{
	UBoxStacker* Stacker = CreateBoxStacker(2, false, 1);
	if (!TestNotNull(TEXT("Stacker created"), Stacker)) return false;

	TArray<int> Shape = { 2, 2, 2 }; // Total Size is 8
	SetupStacker(Stacker, Shape, 0.0f, 16.0f);
	FillStacker(Stacker, Shape);

	FBoxPoint Stacked;
	Stacker->GetStacked(Stacked);

	TestShape(*this, Stacked, {2, 4, 2});
	TestEqual(TEXT("Values.Num() == 16"), Stacked.Values.Num(), 16);

	// Memory layout: etc.
	// First half of Frame 1 (newest) = last pushed: values 8..11
	TestEqual(TEXT("Stacked[0] = frame 1 [0,0,0]"), Stacked.Values[0], 8.0f);
	TestEqual(TEXT("Stacked[1] = frame 1 [0,0,1]"), Stacked.Values[1], 9.0f);
	TestEqual(TEXT("Stacked[2] = frame 1 [0,1,0]"), Stacked.Values[2], 10.0f);
	TestEqual(TEXT("Stacked[3] = frame 1 [0,1,1]"), Stacked.Values[3], 11.0f);
	// Frame 0 (oldest) = first pushed: values 0..3
	TestEqual(TEXT("Stacked[4] = frame 0 [0,0,0]"), Stacked.Values[4], 0.0f);
	TestEqual(TEXT("Stacked[5] = frame 0 [0,0,1]"), Stacked.Values[5], 1.0f);
	TestEqual(TEXT("Stacked[6] = frame 0 [0,1,0]"), Stacked.Values[6], 2.0f);
	TestEqual(TEXT("Stacked[7] = frame 0 [0,1,1]"), Stacked.Values[7], 3.0f);
    // Second half of Frame 1 (newest) = last pushed: values 12..15
	TestEqual(TEXT("Stacked[0] = frame 1 [1,0,0]"), Stacked.Values[8], 12.0f);
	TestEqual(TEXT("Stacked[1] = frame 1 [1,0,1]"), Stacked.Values[9], 13.0f);
	TestEqual(TEXT("Stacked[2] = frame 1 [1,1,0]"), Stacked.Values[10], 14.0f);
	TestEqual(TEXT("Stacked[3] = frame 1 [1,1,1]"), Stacked.Values[11], 15.0f);
	// Second half of Frame 0 (oldest) = first pushed: values 4..7
	TestEqual(TEXT("Stacked[4] = frame 0 [1,0,0]"), Stacked.Values[12], 4.0f);
	TestEqual(TEXT("Stacked[5] = frame 0 [1,0,1]"), Stacked.Values[13], 5.0f);
	TestEqual(TEXT("Stacked[6] = frame 0 [1,1,0]"), Stacked.Values[14], 6.0f);
	TestEqual(TEXT("Stacked[7] = frame 0 [1,1,1]"), Stacked.Values[15], 7.0f);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBoxStacker_PushAddDim_1, "Schola.Adapters.BoxStacker.Push.Push Add New Dim 1", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FBoxStacker_PushAddDim_1::RunTest(const FString& Parameters)
{
	UBoxStacker* Stacker = CreateBoxStacker(2, true, 1);
	if (!TestNotNull(TEXT("Stacker created"), Stacker)) return false;

	TArray<int> Shape = { 2, 2, 2 }; // Total Size is 8
	SetupStacker(Stacker, Shape, 0.0f, 16.0f);
	FillStacker(Stacker, Shape);

	FBoxPoint Stacked;
	Stacker->GetStacked(Stacked);
	// (2 , new_dim, 2 , 2)
	TestShape(*this, Stacked, {2, 2, 2, 2});
	TestEqual(TEXT("Values.Num() == 16"), Stacked.Values.Num(), 16);

	// Memory layout: etc.
	// First half of Frame 1 (newest) = last pushed: values 8..11
	TestEqual(TEXT("Stacked[0] = frame 1 [0,0,0]"), Stacked.Values[0], 8.0f);
	TestEqual(TEXT("Stacked[1] = frame 1 [0,0,1]"), Stacked.Values[1], 9.0f);
	TestEqual(TEXT("Stacked[2] = frame 1 [0,1,0]"), Stacked.Values[2], 10.0f);
	TestEqual(TEXT("Stacked[3] = frame 1 [0,1,1]"), Stacked.Values[3], 11.0f);
	// Frame 0 (oldest) = first pushed: values 0..3
	TestEqual(TEXT("Stacked[4] = frame 0 [0,0,0]"), Stacked.Values[4], 0.0f);
	TestEqual(TEXT("Stacked[5] = frame 0 [0,0,1]"), Stacked.Values[5], 1.0f);
	TestEqual(TEXT("Stacked[6] = frame 0 [0,1,0]"), Stacked.Values[6], 2.0f);
	TestEqual(TEXT("Stacked[7] = frame 0 [0,1,1]"), Stacked.Values[7], 3.0f);
    // Second half of Frame 1 (newest) = last pushed: values 12..15
	TestEqual(TEXT("Stacked[8] = frame 1 [1,0,0]"), Stacked.Values[8], 12.0f);
	TestEqual(TEXT("Stacked[9] = frame 1 [1,0,1]"), Stacked.Values[9], 13.0f);
	TestEqual(TEXT("Stacked[10] = frame 1 [1,1,0]"), Stacked.Values[10], 14.0f);
	TestEqual(TEXT("Stacked[11] = frame 1 [1,1,1]"), Stacked.Values[11], 15.0f);
	// Second half of Frame 0 (oldest) = first pushed: values 4..7
	TestEqual(TEXT("Stacked[12] = frame 0 [1,0,0]"), Stacked.Values[12], 4.0f);
	TestEqual(TEXT("Stacked[13] = frame 0 [1,0,1]"), Stacked.Values[13], 5.0f);
	TestEqual(TEXT("Stacked[14] = frame 0 [1,1,0]"), Stacked.Values[14], 6.0f);
	TestEqual(TEXT("Stacked[15] = frame 0 [1,1,1]"), Stacked.Values[15], 7.0f);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBoxStacker_PushAddDim_Last, "Schola.Adapters.BoxStacker.Push.Push Add New Dim Last", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FBoxStacker_PushAddDim_Last::RunTest(const FString& Parameters)
{
	UBoxStacker* Stacker = CreateBoxStacker(2, true, 3);
	if (!TestNotNull(TEXT("Stacker created"), Stacker)) return false;

	TArray<int> Shape = { 2, 2, 2}; // Total Size is 8
	SetupStacker(Stacker, Shape, 0.0f, 16.0f);
	FillStacker(Stacker, Shape);

	FBoxPoint Stacked;
	Stacker->GetStacked(Stacked);
	// (2 , new_dim, 2 , 2)
	TestShape(*this, Stacked, {2, 2, 2, 2});
	TestEqual(TEXT("Values.Num() == 16"), Stacked.Values.Num(), 16);

	// Memory layout:
	TestEqual(TEXT("Stacked[0] = frame 1 [0,0,0]"), Stacked.Values[0], 8.0f);
	TestEqual(TEXT("Stacked[1] = frame 0 [0,0,0]"), Stacked.Values[1], 0.0f);
	TestEqual(TEXT("Stacked[2] = frame 1 [0,0,1]"), Stacked.Values[2], 9.0f);
	TestEqual(TEXT("Stacked[3] = frame 0 [0,0,1]"), Stacked.Values[3], 1.0f);
	TestEqual(TEXT("Stacked[4] = frame 1 [0,1,0]"), Stacked.Values[4], 10.0f);
	TestEqual(TEXT("Stacked[5] = frame 0 [0,1,0]"), Stacked.Values[5], 2.0f);
	TestEqual(TEXT("Stacked[6] = frame 1 [0,1,1]"), Stacked.Values[6], 11.0f);
	TestEqual(TEXT("Stacked[7] = frame 0 [0,1,1]"), Stacked.Values[7], 3.0f);
	TestEqual(TEXT("Stacked[8] = frame 1 [1,0,0]"), Stacked.Values[8], 12.0f);
	TestEqual(TEXT("Stacked[9] = frame 0 [1,0,0]"), Stacked.Values[9], 4.0f);
	TestEqual(TEXT("Stacked[10] = frame 1 [1,0,1]"), Stacked.Values[10], 13.0f);
	TestEqual(TEXT("Stacked[11] = frame 0 [1,0,1]"), Stacked.Values[11], 5.0f);
	TestEqual(TEXT("Stacked[12] = frame 1 [1,1,0]"), Stacked.Values[12], 14.0f);
	TestEqual(TEXT("Stacked[13] = frame 0 [1,1,0]"), Stacked.Values[13], 6.0f);
	TestEqual(TEXT("Stacked[14] = frame 1 [1,1,1]"), Stacked.Values[14], 15.0f);
	TestEqual(TEXT("Stacked[15] = frame 0 [1,1,1]"), Stacked.Values[15], 7.0f);

	return true;
}


// Verify memory layout: for (1,5,10), Values[0..9]=frame0, Values[10..19]=frame1, etc.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBoxStacker_MemoryLayout_Correct, "Schola.Adapters.BoxStacker.Push.Correct Memory Layout", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FBoxStacker_MemoryLayout_Correct::RunTest(const FString& Parameters)
{
	UBoxStacker* Stacker = CreateBoxStacker(3, true, 1);
	if (!TestNotNull(TEXT("Stacker created"), Stacker)) return false;
	TArray<int> Shape = { 1, 4 };
	SetupStacker(Stacker, Shape, 0.0f, 205.0f);
	for (int32 frame = 0; frame < 3; ++frame)
	{
		TArray<float> Values = { static_cast<float>(frame * 100 + 1), static_cast<float>(frame * 100 + 2), static_cast<float>(frame * 100 + 3), static_cast<float>(frame * 100 + 4) };
		TInstancedStruct<FBoxPoint> InPoint = MakeBoxPointShaped(Values, Shape);
		FInstancedStruct InPointInstanced = ToUntypedInstancedStruct(InPoint);
		FInstancedStruct OutStacked;
		Stacker->Push(InPointInstanced, OutStacked);
	}

	FBoxPoint Stacked;
	Stacker->GetStacked(Stacked);

	// Shape (1,3,4): frame0 at 0-3, frame1 at 4-7, frame2 at 8-11
	TestEqual(TEXT("Shape"), Stacked.Shape, TArray<int>({ 1, 3, 4 }));
	TestEqual(TEXT("Values[0]"), Stacked.Values[0], 201.0f);  // newest frame
	TestEqual(TEXT("Values[3]"), Stacked.Values[3], 204.0f);
	TestEqual(TEXT("Values[4]"), Stacked.Values[4], 101.0f);
	TestEqual(TEXT("Values[8]"), Stacked.Values[8], 1.0f);    // oldest frame

	return true;
}

// Ring overwrite when pushing more than StackSize
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBoxStacker_PushMoreThanStackSize_RingOverwrites, "Schola.Adapters.BoxStacker.Push.Push More Than Stack Size Overwrites", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FBoxStacker_PushMoreThanStackSize_RingOverwrites::RunTest(const FString& Parameters)
{
	UBoxStacker* Stacker = CreateBoxStacker(2);
	if (!TestNotNull(TEXT("Stacker created"), Stacker)) return false;
	SetupStacker(Stacker, {1}, 0.0f, 3.0f);
	// Push 0,1,2
	for (int32 i = 0; i < 3; ++i)
	{
		TInstancedStruct<FBoxPoint> InPoint = MakeBoxPoint(static_cast<float>(i), 1);
		FInstancedStruct InPointInstanced = ToUntypedInstancedStruct(InPoint);
		FInstancedStruct OutStacked;
		Stacker->Push(InPointInstanced, OutStacked);
	}

	TestEqual(TEXT("GetNumValid() == 2"), Stacker->GetNumValid(), 2);

	FBoxPoint Stacked;
	Stacker->GetStacked(Stacked);

	TestEqual(TEXT("Values.Num() == 2"), Stacked.Values.Num(), 2);
	TestEqual(TEXT("Frame 0 (newest) == 2"), Stacked.Values[0], 2.0f);
	TestEqual(TEXT("Frame 1 (oldest) == 1"), Stacked.Values[1], 1.0f);

	return true;
}

// Reset clears NumValid
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBoxStacker_Reset_NumValidZero, "Schola.Adapters.BoxStacker.Reset.Num Valid is Zero After Reset", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FBoxStacker_Reset_NumValidZero::RunTest(const FString& Parameters)
{
	UBoxStacker* Stacker = CreateBoxStacker(4);
	if (!TestNotNull(TEXT("Stacker created"), Stacker)) return false;
	SetupStacker(Stacker, {1, 10});
	TArray<float> Values;
	Values.Init(1.0f,10);
	TInstancedStruct<FBoxPoint> InPoint = MakeBoxPointShaped(Values, {1,10});
	FInstancedStruct InPointInstanced = ToUntypedInstancedStruct(InPoint);
	FInstancedStruct OutStacked;
	Stacker->Push(InPointInstanced, OutStacked);
	TestEqual(TEXT("GetNumValid() == 1 after push"), Stacker->GetNumValid(), 1);

	Stacker->Reset();
	TestEqual(TEXT("GetNumValid() == 0 after reset"), Stacker->GetNumValid(), 0);

	FBoxPoint Stacked;
	Stacker->GetStacked(Stacked);
	TestEqual(TEXT("Stacked still has 4 frames"), Stacked.Values.Num(), 40);

	return true;
}

// GetStacked with empty buffer uses DefaultPoint
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBoxStacker_GetStacked_EmptyBuffer, "Schola.Adapters.BoxStacker.GetStacked.Empty Buffer Uses Default Point", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FBoxStacker_GetStacked_EmptyBuffer::RunTest(const FString& Parameters)
{
	UBoxStacker* Stacker = CreateBoxStacker(4);
	if (!TestNotNull(TEXT("Stacker created"), Stacker)) return false;

	SetupStacker(Stacker, {1, 10});

	FInstancedStruct OutStacked;
	Stacker->GetStacked(OutStacked);

	TestTrue(TEXT("OutStacked valid"), OutStacked.IsValid());
	const FBoxPoint* Box = OutStacked.GetPtr<FBoxPoint>();
	if (!TestNotNull(TEXT("OutStacked is FBoxPoint"), Box)) return false;

	TestShape(*this, *Box, { 4, 1, 10 });
	TestEqual(TEXT("Values.Num() == 40"), Box->Values.Num(), 40);
	for (int32 i = 0; i < Box->Values.Num(); ++i)
	{
		TestEqual(TEXT("All default values 0.5"), Box->Values[i], 0.5f);
	}

	return true;
}

// GetStackedSpace returns correct stacked BoxSpace when adding a new dimension in the middle
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBoxStacker_GetStackedSpace_AddMiddleDim, "Schola.Adapters.BoxStacker.GetStackedSpace.Add Middle Dim", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FBoxStacker_GetStackedSpace_AddMiddleDim::RunTest(const FString& Parameters)
{
	UBoxStacker* Stacker = CreateBoxStacker(5, true, 1);
	if (!TestNotNull(TEXT("Stacker created"), Stacker)) return false;

	SetupStackerWithNonUniformSpace(Stacker, { 1, 10 });

	FBoxSpace OutSpace;
	Stacker->GetStackedSpace(OutSpace);

	TestShape(*this, OutSpace.Shape, TArray<int>({ 1, 5, 10 }));
	TestEqual(TEXT("Dimensions count 50"), OutSpace.Dimensions.Num(), 50);
	// Check that the dimensions are correct
	for (int32 i = 0; i < 50; ++i)
	{
		// The space should count up from 1 to 10 (inclusive), then wrap around
		int	  IntExpectedValue = (i % 10) + 1;
		float ExpectedValue = static_cast<float>(IntExpectedValue);
		TestEqual(TEXT("Dimension " + FString::FromInt(i)), OutSpace.Dimensions[i].Low, -ExpectedValue);
		TestEqual(TEXT("Dimension " + FString::FromInt(i)), OutSpace.Dimensions[i].High, ExpectedValue);
	}

	return true;
}

//GetStackedSpace returns correct shape when adding a new dimension after the last dimension
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBoxStacker_GetStackedSpace_AddNewDimAfterLastDimCorrectShape, "Schola.Adapters.BoxStacker.GetStackedSpace.Add New Dim After Last Dim", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FBoxStacker_GetStackedSpace_AddNewDimAfterLastDimCorrectShape::RunTest(const FString& Parameters)
{
	UBoxStacker* Stacker = CreateBoxStacker(5, true, 2);
	if (!TestNotNull(TEXT("Stacker created"), Stacker)) return false;
	
	SetupStackerWithNonUniformSpace(Stacker, { 1, 10 });

	FBoxSpace OutSpace;
	Stacker->GetStackedSpace(OutSpace);

	TestShape(*this, OutSpace.Shape, TArray<int>({ 1, 10, 5 }));
	TestEqual(TEXT("Dimensions count 50"), OutSpace.Dimensions.Num(), 50);

	for (int32 i = 0; i < 10; ++i)
	{
		for (int32 j = 0; j < 5; ++j)
		{
			// The space should look like [1]*5 + [2]*5 + ... + [10]*5
			int	  IntExpectedValue = (i % 10) + 1;
			float ExpectedValue = static_cast<float>(IntExpectedValue);
			int DstIndex = i*5+j;
			TestEqual(TEXT("Dimension " + FString::FromInt(DstIndex)), OutSpace.Dimensions[DstIndex].Low, -ExpectedValue);
			TestEqual(TEXT("Dimension " + FString::FromInt(DstIndex)), OutSpace.Dimensions[DstIndex].High, ExpectedValue);
		}
	}

	return true;
}

//Get Stacked Space returns correct dimensions when expanding dimension in the middle
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBoxStacker_GetStackedSpace_ExpandMiddleDim, "Schola.Adapters.BoxStacker.GetStackedSpace.Expand Middle Dim", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FBoxStacker_GetStackedSpace_ExpandMiddleDim::RunTest(const FString& Parameters)
{
	UBoxStacker* Stacker = CreateBoxStacker(5, false, 1);
	if (!TestNotNull(TEXT("Stacker created"), Stacker)) return false;
	
	SetupStackerWithNonUniformSpace(Stacker, { 1, 1, 10 });
	
	FBoxSpace OutSpace;
	Stacker->GetStackedSpace(OutSpace);

	TestShape(*this, OutSpace.Shape, TArray<int>({ 1, 5, 10 }));
	TestEqual(TEXT("Dimensions count 50"), OutSpace.Dimensions.Num(), 50);
	// Check that the dimensions are correct
	for (int32 i = 0; i < 50; ++i)
	{
		// The space should count up from 1 to 10 (inclusive), then wrap around
		int	  IntExpectedValue = (i % 10) + 1;
		float ExpectedValue = static_cast<float>(IntExpectedValue);
		TestEqual(TEXT("Dimension " + FString::FromInt(i)), OutSpace.Dimensions[i].Low, -ExpectedValue);
		TestEqual(TEXT("Dimension " + FString::FromInt(i)), OutSpace.Dimensions[i].High, ExpectedValue);
	}

	return true;
}


#endif // WITH_DEV_AUTOMATION_TESTS
