// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"

#define TestEqualExactFloat(TestMessage, Actual, Expected) TestEqual(TestMessage, (float)Actual, (float)Expected, 0.0001f)

//IMPLEMENT_SIMPLE_AUTOMATION_TEST(FNNEPointCreatorDiscreteBufferTest, "Schola.Policies.NNE.NNEPointCreator.DiscreteBuffer Test", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)