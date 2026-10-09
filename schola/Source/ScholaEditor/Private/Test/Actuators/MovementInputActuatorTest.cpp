// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"

#include "Test/Actuators/ActuatorTestWorld.h"

#include "Actuators/MovementInputActuator.h"
#include "AIController.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/DefaultPawn.h"
#include "GameFramework/Pawn.h"
#include "Math/UnrealMathUtility.h"
#include "Points/BoxPoint.h"
#include "Points/DiscretePoint.h"
#include "Spaces/BoxSpace.h"

#if WITH_DEV_AUTOMATION_TESTS

static UMovementInputActuator* CreateMovementInputActuator(AActor* Outer)
{
	return NewObject<UMovementInputActuator>(Outer, NAME_None, RF_Transient);
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMovementInputActuator_GetActionSpace_AllAxes_Test,
	"Schola.Actuators.MovementInput.GetActionSpace.AllAxes",
	GScholaActuatorEditorTestFlags)

bool FMovementInputActuator_GetActionSpace_AllAxes_Test::RunTest(const FString& Parameters)
{
	AActor* Owner = NewObject<AActor>(GetTransientPackage());
	UMovementInputActuator* Actuator = CreateMovementInputActuator(Owner);
	Actuator->MinSpeed = -1.f;
	Actuator->MaxSpeed = 2.f;

	FInstancedStruct SpaceInst;
	Actuator->GetActionSpace_Implementation(SpaceInst);

	TestTrue(TEXT("Action space is BoxSpace"), SpaceInst.GetScriptStruct() == FBoxSpace::StaticStruct());
	const FBoxSpace& Space = SpaceInst.Get<FBoxSpace>();
	TestEqual(TEXT("Three enabled axes -> three dimensions"), Space.Dimensions.Num(), 3);
	TestEqual(TEXT("Low bound"), Space.Dimensions[0].Low, -1.f);
	TestEqual(TEXT("High bound"), Space.Dimensions[2].High, 2.f);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMovementInputActuator_GetActionSpace_XOnly_Test,
	"Schola.Actuators.MovementInput.GetActionSpace.XOnly",
	GScholaActuatorEditorTestFlags)

bool FMovementInputActuator_GetActionSpace_XOnly_Test::RunTest(const FString& Parameters)
{
	AActor* Owner = NewObject<AActor>(GetTransientPackage());
	UMovementInputActuator* Actuator = CreateMovementInputActuator(Owner);
	Actuator->bHasYDimension = false;
	Actuator->bHasZDimension = false;

	FInstancedStruct SpaceInst;
	Actuator->GetActionSpace_Implementation(SpaceInst);
	const FBoxSpace& Space = SpaceInst.Get<FBoxSpace>();
	TestEqual(TEXT("Only X enabled -> one dimension"), Space.Dimensions.Num(), 1);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMovementInputActuator_GenerateId_Test,
	"Schola.Actuators.MovementInput.GenerateId",
	GScholaActuatorEditorTestFlags)

bool FMovementInputActuator_GenerateId_Test::RunTest(const FString& Parameters)
{
	AActor* Owner = NewObject<AActor>(GetTransientPackage());
	UMovementInputActuator* Actuator = CreateMovementInputActuator(Owner);
	const FString Id = Actuator->GenerateId();
	TestTrue(TEXT("Id mentions MovementInput"), Id.Contains(TEXT("MovementInput")));
	TestTrue(TEXT("Id mentions X_true"), Id.Contains(TEXT("X_true")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMovementInputActuator_TakeAction_NonPawnOwner_NoCrash_Test,
	"Schola.Actuators.MovementInput.TakeAction.NonPawnOwner",
	GScholaActuatorEditorTestFlags)

bool FMovementInputActuator_TakeAction_NonPawnOwner_NoCrash_Test::RunTest(const FString& Parameters)
{
	AActor* Owner = NewObject<AActor>(GetTransientPackage());
	UMovementInputActuator* Actuator = CreateMovementInputActuator(Owner);

	AddExpectedMessage(
		TEXT("UMovementInputActuator::TakeAction(): Owner is not a Pawn - cannot apply movement input - MovementInputActuator_0"),
		ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains,
		1,
		false);

	FBoxPoint Action({0.25f, 0.5f, 0.f});
	Actuator->TakeAction(Action);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMovementInputActuator_TakeAction_WrongPointType_Test,
	"Schola.Actuators.MovementInput.TakeAction.WrongPointType",
	GScholaActuatorEditorTestFlags)

bool FMovementInputActuator_TakeAction_WrongPointType_Test::RunTest(const FString& Parameters)
{
	AActor* Owner = NewObject<AActor>(GetTransientPackage());
	UMovementInputActuator* Actuator = CreateMovementInputActuator(Owner);

	AddExpectedMessage(
		TEXT("UMovementInputActuator::TakeAction_Implementation(): Received action is DiscretePoint not a BoxPoint or DictPoint - MovementInputActuator_0"),
		ELogVerbosity::Warning,
		EAutomationExpectedMessageFlags::Contains,
		1,
		false);

	FInstancedStruct InAction;
	InAction.InitializeAs<FDiscretePoint>(FDiscretePoint(0));
	Actuator->TakeAction_Implementation(InAction);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMovementInputActuator_TakeAction_BoxViaInstancedStruct_Test,
	"Schola.Actuators.MovementInput.TakeAction.BoxViaInstancedStruct",
	GScholaActuatorEditorTestFlags)

bool FMovementInputActuator_TakeAction_BoxViaInstancedStruct_Test::RunTest(const FString& Parameters)
{
	AActor* Owner = NewObject<AActor>(GetTransientPackage());
	UMovementInputActuator* Actuator = CreateMovementInputActuator(Owner);

	FInstancedStruct InAction;
	InAction.InitializeAs<FBoxPoint>(FBoxPoint({0.f, 0.f, 0.f}));
	Actuator->TakeAction_Implementation(InAction);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMovementInputActuator_TakeAction_WithDefaultPawn_Test,
	"Schola.Actuators.MovementInput.TakeAction.WithDefaultPawn",
	GScholaActuatorEditorTestFlags)

bool FMovementInputActuator_TakeAction_WithDefaultPawn_Test::RunTest(const FString& Parameters)
{
	FScholaActuatorTestWorld TestWorld;
	if (!TestWorld.Setup(this))
	{
		return false;
	}

	UWorld* World = TestWorld.GetWorld();
	FActorSpawnParameters SpawnParams;
	SpawnParams.ObjectFlags = RF_Transient;
	const FVector Start(100.f, 200.f, 300.f);
	ADefaultPawn* Pawn = World->SpawnActor<ADefaultPawn>(
		ADefaultPawn::StaticClass(),
		FTransform(FRotator::ZeroRotator, Start),
		SpawnParams);
	if (!TestNotNull(TEXT("Spawned DefaultPawn"), Pawn))
	{
		return false;
	}

	// UFloatingPawnMovement::TickComponent only integrates input when Controller->IsLocalController() (see Engine
	// FloatingPawnMovement.cpp). A spawned APlayerController in this test world has no ULocalPlayer, so
	// APlayerController::IsLocalController() is false and movement never runs. AAIController uses AController's
	// implementation, which is true for authority in standalone / server automation worlds.
	FActorSpawnParameters CtrlSpawnParams;
	CtrlSpawnParams.ObjectFlags = RF_Transient;
	CtrlSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AAIController* AIController = World->SpawnActor<AAIController>(AAIController::StaticClass(), Start, FRotator::ZeroRotator, CtrlSpawnParams);
	if (!TestNotNull(TEXT("Spawned AIController for possession"), AIController))
	{
		Pawn->Destroy();
		TestWorld.Tick();
		return false;
	}
	AIController->Possess(Pawn);

	UMovementInputActuator* Actuator = CreateMovementInputActuator(Pawn);
	Actuator->RegisterComponent();
	Actuator->ScaleValue = 1.f;

	const FVector Forward = Pawn->GetActorForwardVector();

	FBoxPoint Action({1.f, 0.f, 0.f});
	Actuator->TakeAction(Action);

	// AddMovementInput is accumulated then consumed by the pawn movement component during world ticks.
	for (int32 i = 0; i < 8; ++i)
	{
		TestWorld.Tick();
	}

	const FVector Displacement = Pawn->GetActorLocation() - Start;
	const float AlongForward = FVector::DotProduct(Displacement, Forward);
	if (!TestTrue(TEXT("Pawn moved along forward axis after movement input"), AlongForward > KINDA_SMALL_NUMBER))
	{
		AIController->UnPossess();
		AIController->Destroy();
		Pawn->Destroy();
		TestWorld.Tick();
		return false;
	}

	AIController->UnPossess();
	AIController->Destroy();
	Pawn->Destroy();
	TestWorld.Tick();

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
