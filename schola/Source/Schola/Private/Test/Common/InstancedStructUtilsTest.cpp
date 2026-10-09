// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "Common/InstancedStructUtils.h"
#include "Points/DiscretePoint.h"
#include "Points/BoxPoint.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FInstancedStructUtils_ToUntyped_LValueRef,
    "Schola.Common.InstancedStructUtils.ToUntypedInstancedStruct.LValue aliases original",
    EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FInstancedStructUtils_ToUntyped_LValueRef::RunTest(const FString& Parameters)
{
    TInstancedStruct<FDiscretePoint> Typed;
    Typed.InitializeAs<FDiscretePoint>(7);

    FInstancedStruct& Untyped = ToUntypedInstancedStruct(Typed);

    TestTrue(TEXT("Untyped is same address as Typed"),
        &Untyped == reinterpret_cast<FInstancedStruct*>(&Typed));
    TestEqual(TEXT("Value round-trips through untyped ref"),
        Untyped.GetPtr<FDiscretePoint>()->Value, 7);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FInstancedStructUtils_ToUntyped_ConstLValueRef,
    "Schola.Common.InstancedStructUtils.ToUntypedInstancedStruct.ConstLValue aliases original",
    EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FInstancedStructUtils_ToUntyped_ConstLValueRef::RunTest(const FString& Parameters)
{
    TInstancedStruct<FDiscretePoint> Typed;
    Typed.InitializeAs<FDiscretePoint>(42);
    const TInstancedStruct<FDiscretePoint>& ConstTyped = Typed;

    const FInstancedStruct& Untyped = ToUntypedInstancedStruct(ConstTyped);

    TestTrue(TEXT("Const untyped is same address as Typed"),
        &Untyped == reinterpret_cast<const FInstancedStruct*>(&Typed));
    TestEqual(TEXT("Value readable through const untyped ref"),
        Untyped.GetPtr<FDiscretePoint>()->Value, 42);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FInstancedStructUtils_ToUntyped_MutationVisible,
    "Schola.Common.InstancedStructUtils.ToUntypedInstancedStruct.Mutation visible through alias",
    EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FInstancedStructUtils_ToUntyped_MutationVisible::RunTest(const FString& Parameters)
{
    TInstancedStruct<FDiscretePoint> Typed;
    Typed.InitializeAs<FDiscretePoint>(1);
    FInstancedStruct& Untyped = ToUntypedInstancedStruct(Typed);

    Typed.GetMutable<FDiscretePoint>().Value = 99;

    TestEqual(TEXT("Mutation on Typed is visible through Untyped alias"),
        Untyped.GetPtr<FDiscretePoint>()->Value, 99);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FInstancedStructUtils_ToTyped_LValueRef,
    "Schola.Common.InstancedStructUtils.ToTypedInstancedStruct.LValue aliases original",
    EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FInstancedStructUtils_ToTyped_LValueRef::RunTest(const FString& Parameters)
{
    FInstancedStruct Untyped;
    Untyped.InitializeAs<FDiscretePoint>(13);

    TInstancedStruct<FDiscretePoint>& Typed = ToTypedInstancedStruct<FDiscretePoint>(Untyped);

    TestTrue(TEXT("Typed is same address as Untyped"),
        reinterpret_cast<FInstancedStruct*>(&Typed) == &Untyped);
    TestEqual(TEXT("Value readable through typed ref"),
        Typed.Get<FDiscretePoint>().Value, 13);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FInstancedStructUtils_ToTyped_ConstLValueRef,
    "Schola.Common.InstancedStructUtils.ToTypedInstancedStruct.ConstLValue aliases original",
    EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FInstancedStructUtils_ToTyped_ConstLValueRef::RunTest(const FString& Parameters)
{
    FInstancedStruct Untyped;
    Untyped.InitializeAs<FDiscretePoint>(55);
    const FInstancedStruct& ConstUntyped = Untyped;

    const TInstancedStruct<FDiscretePoint>& Typed = ToTypedInstancedStruct<FDiscretePoint>(ConstUntyped);

    TestTrue(TEXT("Const typed is same address as Untyped"),
        reinterpret_cast<const FInstancedStruct*>(&Typed) == &Untyped);
    TestEqual(TEXT("Value readable through const typed ref"),
        Typed.Get<FDiscretePoint>().Value, 55);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FInstancedStructUtils_ToTyped_UninitializedPassesThrough,
    "Schola.Common.InstancedStructUtils.ToTypedInstancedStruct.Uninitialized passes through unchecked",
    EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FInstancedStructUtils_ToTyped_UninitializedPassesThrough::RunTest(const FString& Parameters)
{
    FInstancedStruct Untyped;

    TInstancedStruct<FDiscretePoint>& Typed = ToTypedInstancedStruct<FDiscretePoint>(Untyped);

    TestFalse(TEXT("Typed.IsValid() == false for uninitialized struct"), Typed.IsValid());
    return true;
}

#endif
