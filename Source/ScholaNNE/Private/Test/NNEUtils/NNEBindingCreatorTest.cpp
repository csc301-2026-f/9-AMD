// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"

#include "NNE.h"
#include "NNETypes.h"
#include "NNEUtils/NNEBindingCreator.h"
#include "NNEUtils/NNEBuffer.h"
#include "Spaces/BoxSpace.h"
#include "Spaces/DictSpace.h"
#include "Spaces/MultiBinarySpace.h"
#include "Spaces/DiscreteSpace.h"

#define TestEqualExactFloat(TestMessage, Actual, Expected) TestEqual(TestMessage, (float)Actual, (float)Expected, 0.0001f)

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNNEBindingCreatorConstructorTest, "Schola.Policies.NNE.NNEBindingCreator.Constructor Test", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FNNEBindingCreatorConstructorTest::RunTest(const FString& Parameters)
{
    TInstancedStruct<FNNEPointBuffer> TestBuffer;
    TestBuffer.InitializeAs<FNNEBoxBuffer>(3);

    TArray<UE::NNE::FTensorDesc> TensorDescs;
    const UE::NNE::FSymbolicTensorShape ShapeInput = UE::NNE::FSymbolicTensorShape::Make({3});
    TensorDescs.Add(UE::NNE::FTensorDesc::Make(TEXT("input"), ShapeInput, ENNETensorDataType::Float));

    TArray<UE::NNE::FTensorBindingCPU> Bindings;
    Bindings.SetNum(1);

    FNNEBindingCreator Creator(TestBuffer, TensorDescs, Bindings);
    TestEqual(TEXT("bError should be false initially"), Creator.bError, false);
    TestEqual(TEXT("Index should be 0 initially"), Creator.Index, 0);
    TestEqual(TEXT("TensorDescs should be stored"), Creator.TensorDescs.Num(), 1);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNNEBindingCreatorBoxBufferTest, "Schola.Policies.NNE.NNEBindingCreator.BoxBuffer Test", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FNNEBindingCreatorBoxBufferTest::RunTest(const FString& Parameters)
{
    TInstancedStruct<FNNEPointBuffer> Holder;
    Holder.InitializeAs<FNNEBoxBuffer>(3);
    TArray<UE::NNE::FTensorDesc> TensorDescs;
    const UE::NNE::FSymbolicTensorShape ShapeInput = UE::NNE::FSymbolicTensorShape::Make({3});
    TensorDescs.Add(UE::NNE::FTensorDesc::Make(TEXT("input"), ShapeInput, ENNETensorDataType::Float));
    TArray<UE::NNE::FTensorBindingCPU> Bindings; Bindings.SetNum(1);
    FNNEBindingCreator Creator(Holder, TensorDescs, Bindings);

    FNNEBoxBuffer BoxBuffer(3);
    BoxBuffer.Buffer[0] = 1.5f;
    BoxBuffer.Buffer[1] = -2.5f;
    BoxBuffer.Buffer[2] = 3.0f;
    Creator(BoxBuffer);

    TestEqualExactFloat(TEXT("Binding size"), Bindings[0].SizeInBytes, (3 * sizeof(float)));

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNNEBindingCreatorBinaryBufferTest, "Schola.Policies.NNE.NNEBindingCreator.BinaryBuffer Test", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FNNEBindingCreatorBinaryBufferTest::RunTest(const FString& Parameters)
{
    TInstancedStruct<FNNEPointBuffer> Holder; Holder.InitializeAs<FNNEMultiBinaryBuffer>(2);
    TArray<UE::NNE::FTensorDesc> TensorDescs;
    const UE::NNE::FSymbolicTensorShape ShapeBinary = UE::NNE::FSymbolicTensorShape::Make({2});
    TensorDescs.Add(UE::NNE::FTensorDesc::Make(TEXT("binary_input"), ShapeBinary, ENNETensorDataType::Float));
    TArray<UE::NNE::FTensorBindingCPU> Bindings; Bindings.SetNum(1);
    FNNEBindingCreator Creator(Holder, TensorDescs, Bindings);

    FNNEMultiBinaryBuffer BinaryBuffer(2);
    BinaryBuffer.Buffer[0] = true;
    BinaryBuffer.Buffer[1] = false;
    Creator(BinaryBuffer);

    TestEqual(TEXT("Binding size"), Bindings[0].SizeInBytes, (uint64)(2 * sizeof(bool)));
    TestEqual(TEXT("Binding data ptr"), Bindings[0].Data, (void*)BinaryBuffer.Buffer.GetData());
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNNEBindingCreatorDiscreteBufferTest, "Schola.Policies.NNE.NNEBindingCreator.DiscreteBuffer Test", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FNNEBindingCreatorDiscreteBufferTest::RunTest(const FString& Parameters)
{
    TInstancedStruct<FNNEPointBuffer> Holder; Holder.InitializeAs<FNNEDiscreteBuffer>();
    TArray<UE::NNE::FTensorDesc> TensorDescs;
    const UE::NNE::FSymbolicTensorShape ShapeDiscrete = UE::NNE::FSymbolicTensorShape::Make({1});
    TensorDescs.Add(UE::NNE::FTensorDesc::Make(TEXT("discrete_input"), ShapeDiscrete, ENNETensorDataType::Int32));
    TArray<UE::NNE::FTensorBindingCPU> Bindings; Bindings.SetNum(1);
    FNNEBindingCreator Creator(Holder, TensorDescs, Bindings);

    FNNEDiscreteBuffer Buffer;
	Creator(Buffer);

    TestEqual(TEXT("Binding size"), Bindings[0].SizeInBytes, (uint64)(sizeof(int64)));
	TestEqual(TEXT("Binding data ptr"), Bindings[0].Data, (void*)Buffer.Buffer.GetData());
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNNEBindingCreatorDictSkipsStateTensorsTest, "Schola.Policies.NNE.NNEBindingCreator.Dict skips state tensors", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FNNEBindingCreatorDictSkipsStateTensorsTest::RunTest(const FString& Parameters)
{
    // Dict buffer with a regular input only
    TMap<FString, TInstancedStruct<FNNEPointBuffer>> Map;
    TInstancedStruct<FNNEPointBuffer> Regular; Regular.InitializeAs<FNNEBoxBuffer>(2);
    Map.Add(TEXT("regular_input"), Regular);

    TInstancedStruct<FNNEPointBuffer> Root; Root.InitializeAs<FNNEDictBuffer>(FNNEDictBuffer(Map));

    // Tensor descs include state_in/out around a regular tensor
    TArray<UE::NNE::FTensorDesc> TensorDescs;
    const UE::NNE::FSymbolicTensorShape ShapeState64 = UE::NNE::FSymbolicTensorShape::Make({64});
    const UE::NNE::FSymbolicTensorShape ShapeReg2 = UE::NNE::FSymbolicTensorShape::Make({2});
    TensorDescs.Add(UE::NNE::FTensorDesc::Make(TEXT("state_in_hidden"), ShapeState64, ENNETensorDataType::Float));
    TensorDescs.Add(UE::NNE::FTensorDesc::Make(TEXT("regular_input"), ShapeReg2, ENNETensorDataType::Float));
    TensorDescs.Add(UE::NNE::FTensorDesc::Make(TEXT("state_out_cell"), ShapeState64, ENNETensorDataType::Float));

    TArray<UE::NNE::FTensorBindingCPU> Bindings; Bindings.SetNum(3);
    FNNEBindingCreator Creator(Root, TensorDescs, Bindings);

    FNNEDictBuffer Dict(Map);
    Creator(Dict);

    TestEqual(TEXT("Should not error"), Creator.bError, false);
    TestEqual(TEXT("Processed all descs"), Creator.Index, 3);
    // Only middle binding should be set by Box buffer
    TestEqual(TEXT("Middle binding size"), Bindings[1].SizeInBytes, (uint64)(2 * sizeof(float)));
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNNEBindingCreatorMissingKeyTest, "Schola.Policies.NNE.NNEBindingCreator.Missing key error", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FNNEBindingCreatorMissingKeyTest::RunTest(const FString& Parameters)
{
    // Dict with different key than tensor desc
    TMap<FString, TInstancedStruct<FNNEPointBuffer>> Map;
    TInstancedStruct<FNNEPointBuffer> Buf; Buf.InitializeAs<FNNEBoxBuffer>(2);
    Map.Add(TEXT("existing_key"), Buf);

    TInstancedStruct<FNNEPointBuffer> Root; Root.InitializeAs<FNNEDictBuffer>(FNNEDictBuffer(Map));

    TArray<UE::NNE::FTensorDesc> TensorDescs;
    const UE::NNE::FSymbolicTensorShape ShapeTwo = UE::NNE::FSymbolicTensorShape::Make({2});
    TensorDescs.Add(UE::NNE::FTensorDesc::Make(TEXT("missing_key"), ShapeTwo, ENNETensorDataType::Float));

    TArray<UE::NNE::FTensorBindingCPU> Bindings; Bindings.SetNum(1);
    FNNEBindingCreator Creator(Root, TensorDescs, Bindings);

    FNNEDictBuffer Dict(Map);
    // Expect the source to log an error when the key is missing; mark it as expected so the test doesn't fail on the log.
    AddExpectedError(TEXT("Key missing_key not found or invalid in Dict Buffer"), EAutomationExpectedErrorFlags::Contains, 1);
    Creator(Dict);
    TestEqual(TEXT("Should error on missing key"), Creator.bError, true);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNNEBindingCreatorOutOfBoundsTest, "Schola.Policies.NNE.NNEBindingCreator.Out of bounds bindings", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FNNEBindingCreatorOutOfBoundsTest::RunTest(const FString& Parameters)
{
    // Dict with a single buffer but two tensor descs; bindings array too small
	AddExpectedError(TEXT("LogScholaNNE: .*"), EAutomationExpectedErrorFlags::Contains, 1);
    TMap<FString, TInstancedStruct<FNNEPointBuffer>> Map;
    TInstancedStruct<FNNEPointBuffer> Buf; Buf.InitializeAs<FNNEBoxBuffer>(2);
    Map.Add(TEXT("input"), Buf);
    TInstancedStruct<FNNEPointBuffer> Root; Root.InitializeAs<FNNEDictBuffer>(FNNEDictBuffer(Map));

    TArray<UE::NNE::FTensorDesc> TensorDescs;
    const UE::NNE::FSymbolicTensorShape ShapeTwoA = UE::NNE::FSymbolicTensorShape::Make({2});
    const UE::NNE::FSymbolicTensorShape ShapeTwoB = UE::NNE::FSymbolicTensorShape::Make({2});
    TensorDescs.Add(UE::NNE::FTensorDesc::Make(TEXT("input"), ShapeTwoA, ENNETensorDataType::Float));
    TensorDescs.Add(UE::NNE::FTensorDesc::Make(TEXT("input2"), ShapeTwoB, ENNETensorDataType::Float));

    TArray<UE::NNE::FTensorBindingCPU> Bindings; Bindings.SetNum(1);
    FNNEBindingCreator Creator(Root, TensorDescs, Bindings);
    FNNEDictBuffer Dict(Map);
    Creator(Dict);
    TestEqual(TEXT("Creator should be marked as having an error"), Creator.bError, true);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNNEBindingCreatorCreateBindingsSmokeTest, "Schola.Policies.NNE.NNEBindingCreator.CreateBindings Smoke", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FNNEBindingCreatorCreateBindingsSmokeTest::RunTest(const FString& Parameters)
{
    // Space is currently unused, but provide a concrete type
    TInstancedStruct<FSpace> Space; Space.InitializeAs<FBoxSpace>();

    TInstancedStruct<FNNEPointBuffer> Buffer; Buffer.InitializeAs<FNNEBoxBuffer>(3);
    Buffer.GetMutable<FNNEBoxBuffer>().Buffer[0] = 1.0f;
    Buffer.GetMutable<FNNEBoxBuffer>().Buffer[1] = 2.0f;
    Buffer.GetMutable<FNNEBoxBuffer>().Buffer[2] = 3.0f;

    TArray<UE::NNE::FTensorDesc> TensorDescs;
    const UE::NNE::FSymbolicTensorShape ShapeThree = UE::NNE::FSymbolicTensorShape::Make({3});
    TensorDescs.Add(UE::NNE::FTensorDesc::Make(TEXT("input"), ShapeThree, ENNETensorDataType::Float));

    TArray<UE::NNE::FTensorBindingCPU> Bindings; Bindings.SetNum(1);
    bool bError = FNNEBindingCreator::CreateBindings(Space, Buffer, TensorDescs, Bindings);

    TestEqual(TEXT("Should not report error"), bError, false);
    TestEqual(TEXT("Binding size"), Bindings[0].SizeInBytes, (uint64)(3 * sizeof(float)));
    TestEqual(TEXT("Binding data"), Bindings[0].Data, (void*)Buffer.GetMutable<FNNEBoxBuffer>().Buffer.GetData());
    return true;
}