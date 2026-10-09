// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "Adapters/DictStacker.h"
#include "Points/DictPoint.h"
#include "Points/DiscretePoint.h"
#include "Points/BoxPoint.h"
#include "Spaces/DictSpace.h"
#include "Spaces/DiscreteSpace.h"
#include "Spaces/BoxSpace.h"
#include "Points/Blueprint/DictPointBlueprintLibrary.h"
#include "Points/Blueprint/DiscretePointBlueprintLibrary.h"
#include "Spaces/Blueprint/DiscreteSpaceBlueprintLibrary.h"
#include "Points/Blueprint/BoxPointBlueprintLibrary.h"
#include "Spaces/Blueprint/DictSpaceBlueprintLibrary.h"
#include "Spaces/Blueprint/BoxSpaceBlueprintLibrary.h"
#include "Common/InstancedStructUtils.h"

#if WITH_DEV_AUTOMATION_TESTS

static UDictStacker* CreateDictStacker(int32 StackSize = 4)
{
	UDictStacker* Stacker = NewObject<UDictStacker>();
	if (Stacker)
	{
		Stacker->StackSize = StackSize;
	}
	return Stacker;
}

static int32 GetDictEntryAsInt(const FDictPoint& Dict, const FString& Key)
{
	const TInstancedStruct<FPoint>* Point = Dict.Points.Find(Key);
	const FDiscretePoint* Discrete = Point ? Point->GetPtr<FDiscretePoint>() : nullptr;
	return Discrete ? Discrete->Value : 0;
}

static int32 GetDictEntryAsInt(const FInstancedStruct& Dict, const FString& Key)
{
	const FDictPoint* DictPoint = Dict.GetPtr<FDictPoint>();
	return DictPoint ? GetDictEntryAsInt(*DictPoint, Key) : 0;
}

static void MakeDictPointWithKey(const FString& Key, int32 DiscreteValue, FInstancedStruct& OutInstancedPoint)
{
	TInstancedStruct<FPoint> DiscretePoint = UDiscretePointBlueprintLibrary::Int32ToDiscretePoint(DiscreteValue);
	OutInstancedPoint.InitializeAs<FDictPoint>();
	OutInstancedPoint.GetMutable<FDictPoint>().Points.Add(Key, DiscretePoint);
}

static void MakeDictSpaceWithKey(const FString& Key, int32 DiscreteMax, TInstancedStruct<FSpace>& OutInstancedSpace)
{
	TInstancedStruct<FDiscreteSpace> DiscreteSpace = UDiscreteSpaceBlueprintLibrary::Int32ToDiscreteSpace(DiscreteMax);
	OutInstancedSpace.InitializeAs<FDictSpace>();
	OutInstancedSpace.GetMutable<FDictSpace>().Spaces.Add(Key, DiscreteSpace);
}

// Push single entry, GetNumValid == 1
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDictStacker_PushSingle_NumValidOne, "Schola.Adapters.DictStacker.Push.SingleNumValid", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FDictStacker_PushSingle_NumValidOne::RunTest(const FString& Parameters)
{
	UDictStacker* Stacker = CreateDictStacker(4);
	if (!TestNotNull(TEXT("Stacker created"), Stacker)) return false;

	FInstancedStruct InPoint;
	MakeDictPointWithKey(TEXT("a"), 5, InPoint);
	MakeDictSpaceWithKey(TEXT("a"), 10, Stacker->UnstackedSpace);
	MakeDictPointWithKey(TEXT("a"), 1, ToUntypedInstancedStruct(Stacker->DefaultPoint));
	FInstancedStruct OutStacked;
	Stacker->Push(InPoint, OutStacked);

	TestEqual(TEXT("GetNumValid() == 1"), Stacker->GetNumValid(), 1);
	return true;
}

// Push DictPoint, GetStacked has key_0
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDictStacker_PushDictPoint_GetStackedHasSuffixedKey, "Schola.Adapters.DictStacker.Push.DictPointSuffixedKey", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FDictStacker_PushDictPoint_GetStackedHasSuffixedKey::RunTest(const FString& Parameters)
{
	UDictStacker* Stacker = CreateDictStacker(4);
	if (!TestNotNull(TEXT("Stacker created"), Stacker)) return false;

	FInstancedStruct InPoint;
	MakeDictPointWithKey(TEXT("action"), 7, InPoint);
	MakeDictSpaceWithKey(TEXT("action"), 10, Stacker->UnstackedSpace);
	MakeDictPointWithKey(TEXT("action"), 1, ToUntypedInstancedStruct(Stacker->DefaultPoint));
	FInstancedStruct OutStacked;
	Stacker->Push(InPoint, OutStacked);

	TestTrue(TEXT("OutStacked is valid"), OutStacked.IsValid());
	const FDictPoint* Dict = OutStacked.GetPtr<FDictPoint>();
	if (!TestNotNull(TEXT("OutStacked is FDictPoint"), Dict)) return false;
	TestTrue(TEXT("Stacked contains action_0"), Dict->Points.Contains(TEXT("action_0")));
	return true;
}

// Push multiple, GetStacked has key_0, key_1, key_2, key_3 (key_3 is default slot)
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDictStacker_PushMultiple_GetStackedIndices, "Schola.Adapters.DictStacker.Push.MultipleIndices", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FDictStacker_PushMultiple_GetStackedIndices::RunTest(const FString& Parameters)
{
	UDictStacker* Stacker = CreateDictStacker(4);
	if (!TestNotNull(TEXT("Stacker created"), Stacker)) return false;
	MakeDictPointWithKey(TEXT("x"), 0, ToUntypedInstancedStruct(Stacker->DefaultPoint));
	MakeDictSpaceWithKey(TEXT("x"), 10, Stacker->UnstackedSpace);

	for (int32 i = 1; i < 4; ++i)
	{
		FInstancedStruct InPoint;
		MakeDictPointWithKey(TEXT("x"), i, InPoint);
		FInstancedStruct OutStacked;
		Stacker->Push(InPoint, OutStacked);
	}

	TestEqual(TEXT("GetNumValid() == 3"), Stacker->GetNumValid(), 3);

	FDictPoint Stacked;
	Stacker->GetStacked(Stacked);

	TestEqual(TEXT("StackedPoint['x_0'] == 3"), GetDictEntryAsInt(Stacked, FString("x_0")), 3);
	TestEqual(TEXT("StackedPoint['x_1'] == 2"), GetDictEntryAsInt(Stacked, FString("x_1")), 2);
	TestEqual(TEXT("StackedPoint['x_2'] == 1"), GetDictEntryAsInt(Stacked, FString("x_2")), 1);
	TestEqual(TEXT("StackedPoint['x_3'] == 0 (default slot)"), GetDictEntryAsInt(Stacked, FString("x_3")), 0);
	TestEqual(TEXT("Stacked.Points.Num() == 4 (StackSize)"), Stacked.Points.Num(), 4);
	return true;
}

// StackSize 2, push 3 -> ring overwrites, GetNumValid == 2
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDictStacker_PushMoreThanStackSize_RingOverwrites, "Schola.Adapters.DictStacker.Push.RingOverwrites", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FDictStacker_PushMoreThanStackSize_RingOverwrites::RunTest(const FString& Parameters)
{
	UDictStacker* Stacker = CreateDictStacker(2);
	if (!TestNotNull(TEXT("Stacker created"), Stacker)) return false;
	MakeDictPointWithKey(TEXT("k"), 0, ToUntypedInstancedStruct(Stacker->DefaultPoint));
	MakeDictSpaceWithKey(TEXT("k"), 4, Stacker->UnstackedSpace);
	for (int32 i = 1; i < 4; ++i) 
	{
		FInstancedStruct InPoint;
		MakeDictPointWithKey(TEXT("k"), i, InPoint);
		FInstancedStruct OutStacked;
		Stacker->Push(InPoint, OutStacked);
	}

	TestEqual(TEXT("GetNumValid() == 2"), Stacker->GetNumValid(), 2);

	FDictPoint Stacked;
	Stacker->GetStacked(Stacked);
	// Should have k_0 and k_1 (third and second values of the ring after 3 pushes = values 2 and 1)
	TestEqual(TEXT("Stacked.Points.Num() == 2"), Stacked.Points.Num(), 2);
	TestEqual(TEXT("StackedPoint['k_0'] == 3"), GetDictEntryAsInt(Stacked, FString("k_0")), 3);
	TestEqual(TEXT("StackedPoint['k_1'] == 2"), GetDictEntryAsInt(Stacked, FString("k_1")), 2);
	return true;
}

// Reset clears real entries; buffer is refilled with DefaultPoint and always returns StackSize slots
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDictStacker_Reset_NumValidZero, "Schola.Adapters.DictStacker.Reset.NumValidZero", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FDictStacker_Reset_NumValidZero::RunTest(const FString& Parameters)
{
	UDictStacker* Stacker = CreateDictStacker(4);
	if (!TestNotNull(TEXT("Stacker created"), Stacker)) return false;
	MakeDictPointWithKey(TEXT("x"), 0, ToUntypedInstancedStruct(Stacker->DefaultPoint));
	MakeDictSpaceWithKey(TEXT("x"), 10, Stacker->UnstackedSpace);

	FInstancedStruct InPoint;
	MakeDictPointWithKey(TEXT("x"), 1, InPoint);
	FInstancedStruct OutStacked;
	Stacker->Push(InPoint, OutStacked);
	TestEqual(TEXT("GetNumValid() == 1 after push"), Stacker->GetNumValid(), 1);

	Stacker->Reset();
	TestEqual(TEXT("GetNumValid() == 0 after reset"), Stacker->GetNumValid(), 0);

	FDictPoint Stacked;
	Stacker->GetStacked(Stacked);
	// After reset, stacked output still has StackSize slots (all defaults)
	TestEqual(TEXT("Stacked has 4 slots (a_0..a_3) after reset"), Stacked.Points.Num(), 4);
	return true;
}

// GetStacked with no pushes yields StackSize slots filled with DefaultPoint
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDictStacker_GetStacked_EmptyBuffer, "Schola.Adapters.DictStacker.GetStacked.EmptyBuffer", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FDictStacker_GetStacked_EmptyBuffer::RunTest(const FString& Parameters)
{
	UDictStacker* Stacker = CreateDictStacker(4);
	if (!TestNotNull(TEXT("Stacker created"), Stacker)) return false;
	MakeDictPointWithKey(TEXT("x"), 0, ToUntypedInstancedStruct(Stacker->DefaultPoint));
	MakeDictSpaceWithKey(TEXT("x"), 10, Stacker->UnstackedSpace);

	FInstancedStruct OutStacked;
	Stacker->GetStacked(OutStacked);
	TestTrue(TEXT("OutStacked valid"), OutStacked.IsValid());
	const FDictPoint* Dict = OutStacked.GetPtr<FDictPoint>();
	if (!TestNotNull(TEXT("OutStacked is FDictPoint"), Dict)) return false;
	// Buffer initially has StackSize default slots
	TestEqual(TEXT("Stacked has 4 slots (x_0..x_3)"), Dict->Points.Num(), 4);
	// Check default values
	TestEqual(TEXT("StackedPoint['x_0'] == 0"), GetDictEntryAsInt(OutStacked, FString("x_0")), 0);
	TestEqual(TEXT("StackedPoint['x_1'] == 0"), GetDictEntryAsInt(OutStacked, FString("x_1")), 0);
	TestEqual(TEXT("StackedPoint['x_2'] == 0"), GetDictEntryAsInt(OutStacked, FString("x_2")), 0);
	TestEqual(TEXT("StackedPoint['x_3'] == 0"), GetDictEntryAsInt(OutStacked, FString("x_3")), 0);

	return true;
}

// Push non-DictPoint (BoxPoint) -> GetStacked has "Point_0"
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDictStacker_PushNonDictPoint_PointSuffix, "Schola.Adapters.DictStacker.Push.NonDictPointSuffix", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FDictStacker_PushNonDictPoint_PointSuffix::RunTest(const FString& Parameters)
{
	UDictStacker* Stacker = CreateDictStacker(4);
	if (!TestNotNull(TEXT("Stacker created"), Stacker)) return false;
	Stacker->DefaultPoint = UBoxPointBlueprintLibrary::ArrayToBoxPoint({ 0.0f, 0.0f });
	Stacker->UnstackedSpace = UBoxSpaceBlueprintLibrary::ArraysToBoxSpace({ 0.0f, 0.0f }, { 10.0f, 10.0f }, {2});

	TInstancedStruct<FPoint> TemplatedInstancedPoint = UBoxPointBlueprintLibrary::ArrayToBoxPoint({ 1.0f, 2.0f });
	FInstancedStruct& InPoint = ToUntypedInstancedStruct(TemplatedInstancedPoint);
	FInstancedStruct OutStacked;
	Stacker->Push(InPoint, OutStacked);

	const FDictPoint* Dict = OutStacked.GetPtr<FDictPoint>();
	if (!TestNotNull(TEXT("OutStacked is FDictPoint"), Dict)) return false;
	TestTrue(TEXT("Contains Point_0"), Dict->Points.Contains(TEXT("Point_0")));
	TestEqual(TEXT("Expected 4 Entries"), Dict->Points.Num(), 4);
	// Check the value of Point_0 is the BoxPoint we pushed (compare first float value)
	const TInstancedStruct<FPoint>* Point = Dict->Points.Find(TEXT("Point_0"));
	if (!TestNotNull(TEXT("Point_0 is valid"), Point)) return false;
	TestEqual(TEXT("Point_0 == [1.0, 2.0]"), Point->Get<FBoxPoint>().Values, { 1.0f, 2.0f });
	
	return true;
}

// Push four times with StackSize 3; after each push verify ordering: Point_0 = newest, highest index = oldest
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDictStacker_PushFour_OrderingCorrect, "Schola.Adapters.DictStacker.Push.OrderingCorrect", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FDictStacker_PushFour_OrderingCorrect::RunTest(const FString& Parameters)
{
	UDictStacker* Stacker = CreateDictStacker(3);
	if (!TestNotNull(TEXT("Stacker created"), Stacker)) return false;
	MakeDictPointWithKey(TEXT("v"), 0, ToUntypedInstancedStruct(Stacker->DefaultPoint));
	MakeDictSpaceWithKey(TEXT("v"), 50, Stacker->UnstackedSpace);

	// Push 1: value 10 -> only Point_0 (newest = only entry)
	{
		FInstancedStruct In;
		MakeDictPointWithKey(TEXT("v"), 10, In);
		FInstancedStruct Out;
		Stacker->Push(In, Out);
		FDictPoint Stacked;
		Stacker->GetStacked(Stacked);
		TestEqual(TEXT("After push 1: NumValid"), Stacker->GetNumValid(), 1);
		TestTrue(TEXT("After push 1: has v_0"), Stacked.Points.Contains(TEXT("v_0")));
		TestEqual(TEXT("After push 1: v_0 (newest) = 10"), GetDictEntryAsInt(Stacked, TEXT("v_0")), 10);
	}

	// Push 2: value 20 -> v_0 (newest)=20, v_1 (oldest)=10
	{
		FInstancedStruct In;
		MakeDictPointWithKey(TEXT("v"), 20, In);
		FInstancedStruct Out;
		Stacker->Push(In, Out);
		FDictPoint Stacked;
		Stacker->GetStacked(Stacked);
		TestEqual(TEXT("After push 2: NumValid"), Stacker->GetNumValid(), 2);
		TestEqual(TEXT("After push 2: v_0 (newest) = 20"), GetDictEntryAsInt(Stacked, TEXT("v_0")), 20);
		TestEqual(TEXT("After push 2: v_1 (oldest) = 10"), GetDictEntryAsInt(Stacked, TEXT("v_1")), 10);
	}

	// Push 3: value 30 -> v_0=30, v_1=20, v_2=10
	{
		FInstancedStruct In;
		MakeDictPointWithKey(TEXT("v"), 30, In);
		FInstancedStruct Out;
		Stacker->Push(In, Out);
		FDictPoint Stacked;
		Stacker->GetStacked(Stacked);
		TestEqual(TEXT("After push 3: NumValid"), Stacker->GetNumValid(), 3);
		TestEqual(TEXT("After push 3: v_0 (newest) = 30"), GetDictEntryAsInt(Stacked, TEXT("v_0")), 30);
		TestEqual(TEXT("After push 3: v_1 = 20"), GetDictEntryAsInt(Stacked, TEXT("v_1")), 20);
		TestEqual(TEXT("After push 3: v_2 (oldest) = 10"), GetDictEntryAsInt(Stacked, TEXT("v_2")), 10);
	}

	// Push 4: ring overwrites oldest (10). v_0=40 (newest), v_1=30, v_2=20 (oldest)
	{
		FInstancedStruct In;
		MakeDictPointWithKey(TEXT("v"), 40, In);
		FInstancedStruct Out;
		Stacker->Push(In, Out);
		FDictPoint Stacked;
		Stacker->GetStacked(Stacked);
		TestEqual(TEXT("After push 4: NumValid"), Stacker->GetNumValid(), 3);
		TestEqual(TEXT("After push 4: v_0 (newest) = 40"), GetDictEntryAsInt(Stacked, TEXT("v_0")), 40);
		TestEqual(TEXT("After push 4: v_1 = 30"), GetDictEntryAsInt(Stacked, TEXT("v_1")), 30);
		TestEqual(TEXT("After push 4: v_2 (oldest) = 20"), GetDictEntryAsInt(Stacked, TEXT("v_2")), 20);
	}

	return true;
}

// GetStackedSpace with UnstackedSpace FDictSpace -> replicated keys key_0, key_1, ...
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDictStacker_GetStackedSpace_DictSpaceReplicated, "Schola.Adapters.DictStacker.GetStackedSpace.DictSpaceReplicated", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FDictStacker_GetStackedSpace_DictSpaceReplicated::RunTest(const FString& Parameters)
{
	UDictStacker* Stacker = CreateDictStacker(3);
	if (!TestNotNull(TEXT("Stacker created"), Stacker)) return false;

	TMap<FString, TInstancedStruct<FSpace>> Spaces;
	TInstancedStruct<FSpace> DiscreteSpace;
	DiscreteSpace.InitializeAs<FDiscreteSpace>(10);
	Spaces.Add(TEXT("action"), DiscreteSpace);
	Spaces.Add(TEXT("alt_action"), DiscreteSpace);
	Stacker->UnstackedSpace.InitializeAs<FDictSpace>(Spaces);
	
	TInstancedStruct<FPoint> DiscretePoint;
	DiscretePoint.InitializeAs<FDiscretePoint>(5);
	TMap<FString, TInstancedStruct<FPoint>> DefaultPoints;
	DefaultPoints.Add(TEXT("action"), DiscretePoint);
	DefaultPoints.Add(TEXT("alt_action"), DiscretePoint);
	Stacker->DefaultPoint.InitializeAs<FDictPoint>(DefaultPoints);

	FDictSpace OutSpace;
	Stacker->GetStackedSpace(OutSpace);

	TestTrue(TEXT("Contains action_0"), OutSpace.Spaces.Contains(TEXT("action_0")));
	TestTrue(TEXT("Contains action_1"), OutSpace.Spaces.Contains(TEXT("action_1")));
	TestTrue(TEXT("Contains action_2"), OutSpace.Spaces.Contains(TEXT("action_2")));
	TestTrue(TEXT("Contains alt_action_0"), OutSpace.Spaces.Contains(TEXT("alt_action_0")));
	TestTrue(TEXT("Contains alt_action_1"), OutSpace.Spaces.Contains(TEXT("alt_action_1")));
	TestTrue(TEXT("Contains alt_action_2"), OutSpace.Spaces.Contains(TEXT("alt_action_2")));
	TestEqual(TEXT("StackSize 3 -> 6 spaces"), OutSpace.Spaces.Num(), 6);
	return true;
}

// GetStackedSpace FInstancedStruct overload
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FDictStacker_GetStackedSpace_InstancedStruct, "Schola.Adapters.DictStacker.GetStackedSpace.InstancedStruct", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FDictStacker_GetStackedSpace_InstancedStruct::RunTest(const FString& Parameters)
{
	UDictStacker* Stacker = CreateDictStacker(2);
	if (!TestNotNull(TEXT("Stacker created"), Stacker)) return false;

	TMap<FString, FInstancedStruct> Spaces;
	FInstancedStruct BoxSpace;
	BoxSpace.InitializeAs<FBoxSpace>();
	Spaces.Add(TEXT("obs"), BoxSpace);
	Stacker->UnstackedSpace = UDictSpaceBlueprintLibrary::MapToDictSpace(Spaces);

	FInstancedStruct OutSpace;
	Stacker->GetStackedSpace(OutSpace);

	TestTrue(TEXT("OutSpace valid"), OutSpace.IsValid());
	const FDictSpace* Dict = OutSpace.GetPtr<FDictSpace>();
	if (!TestNotNull(TEXT("OutSpace is FDictSpace"), Dict)) return false;
	TestTrue(TEXT("Contains obs_0"), Dict->Spaces.Contains(TEXT("obs_0")));
	TestTrue(TEXT("Contains obs_1"), Dict->Spaces.Contains(TEXT("obs_1")));
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
