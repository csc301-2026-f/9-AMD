// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"

#include "TestStepper.h"
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"

#include "Spaces/BoxSpace.h"
#include "Spaces/MultiDiscreteSpace.h"
#include "Agent/AgentInterface.h"
#include "Steppers/SimpleStepper.h"
#include "Policies/PolicyInterface.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSimpleStepperTest, "Schola.Steppers.SimpleStepper Test", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FSimpleStepperTest::RunTest(const FString& Parameters)
{
	
	FInteractionDefinition AgentDefn;
	UTestAgent* TestAgent = NewObject<UTestAgent>();
	IAgent::Execute_Define(TestAgent, AgentDefn);

	UTestPolicy* TestPolicy = NewObject<UTestPolicy>();

	USimpleStepper* SimpleStepper = NewObject<USimpleStepper>();

	TArray<TScriptInterface<IAgent>> Agents;
	Agents.Add(TestAgent);

	TScriptInterface<IPolicy> PolicyInterface = TestPolicy;

	bool InitStepper = SimpleStepper->Init(Agents, PolicyInterface);

	TestTrue("SimpleStepper initialized successfully", InitStepper);

	SimpleStepper->Step();

	return true;
}