// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"

#include "Test/Actuators/ActuatorTestWorld.h"

#include "Actuators/TeleportActuator.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Points/BoxPoint.h"
#include "Points/MultiDiscretePoint.h"
#include "Spaces/MultiDiscreteSpace.h"

#if WITH_DEV_AUTOMATION_TESTS

static UTeleportActuator* CreateTeleportActuator(AActor* Outer)
{
	return NewObject<UTeleportActuator>(Outer, NAME_None, RF_Transient);
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTeleportActuator_GetActionSpace_AllAxes_Test,
	"Schola.Actuators.Teleport.GetActionSpace.AllAxes",
	GScholaActuatorEditorTestFlags)

bool FTeleportActuator_GetActionSpace_AllAxes_Test::RunTest(const FString& Parameters)
{
	AActor* Owner = NewObject<AActor>(GetTransientPackage());
	UTeleportActuator* Actuator = CreateTeleportActuator(Owner);
	const uint8 Both = static_cast<uint8>(ETeleportDimensionFlags::Forwards) | static_cast<uint8>(ETeleportDimensionFlags::Backwards);
	Actuator->XMovementDirectionFlags = Both;
	Actuator->YMovementDirectionFlags = Both;
	Actuator->ZMovementDirectionFlags = Both;

	FInstancedStruct SpaceInst;
	Actuator->GetActionSpace_Implementation(SpaceInst);

	TestTrue(TEXT("MultiDiscrete space"), SpaceInst.GetScriptStruct() == FMultiDiscreteSpace::StaticStruct());
	const FMultiDiscreteSpace& Space = SpaceInst.Get<FMultiDiscreteSpace>();
	TestEqual(TEXT("Three bidirectional axes -> three discrete dimensions"), Space.High.Num(), 3);
	TestEqual(TEXT("X axis cardinality (forward + backward)"), Space.High[0], 2);
	TestEqual(TEXT("Y axis cardinality (forward + backward)"), Space.High[1], 2);
	TestEqual(TEXT("Z axis cardinality (forward + backward)"), Space.High[2], 2);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTeleportActuator_GetActionSpace_XOnly_Test,
	"Schola.Actuators.Teleport.GetActionSpace.XOnly",
	GScholaActuatorEditorTestFlags)

bool FTeleportActuator_GetActionSpace_XOnly_Test::RunTest(const FString& Parameters)
{
	AActor* Owner = NewObject<AActor>(GetTransientPackage());
	UTeleportActuator* Actuator = CreateTeleportActuator(Owner);
	Actuator->YMovementDirectionFlags = 0;
	Actuator->ZMovementDirectionFlags = 0;

	FInstancedStruct SpaceInst;
	Actuator->GetActionSpace_Implementation(SpaceInst);
	TestTrue(TEXT("MultiDiscrete space"), SpaceInst.GetScriptStruct() == FMultiDiscreteSpace::StaticStruct());
	const FMultiDiscreteSpace& Space = SpaceInst.Get<FMultiDiscreteSpace>();
	TestEqual(TEXT("Only X bidirectional -> one discrete dimension"), Space.High.Num(), 1);
	TestEqual(TEXT("Two movement bits -> cardinality 2"), Space.High[0], 2);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTeleportActuator_GenerateId_Test,
	"Schola.Actuators.Teleport.GenerateId",
	GScholaActuatorEditorTestFlags)

bool FTeleportActuator_GenerateId_Test::RunTest(const FString& Parameters)
{
	AActor* Owner = NewObject<AActor>(GetTransientPackage());
	UTeleportActuator* Actuator = CreateTeleportActuator(Owner);
	const FString Id = Actuator->GenerateId();
	TestTrue(TEXT("Id mentions Teleport"), Id.Contains(TEXT("Teleport")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FTeleportActuator_TakeAction_WrongPointType_Test,
	"Schola.Actuators.Teleport.TakeAction.WrongPointType",
	GScholaActuatorEditorTestFlags)

bool FTeleportActuator_TakeAction_WrongPointType_Test::RunTest(const FString& Parameters)
{
	AActor* Owner = NewObject<AActor>(GetTransientPackage());
	UTeleportActuator* Actuator = CreateTeleportActuator(Owner);

	FInstancedStruct InAction;
	InAction.InitializeAs<FBoxPoint>(FBoxPoint({1.f}));
	Actuator->TakeAction_Implementation(InAction);

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
