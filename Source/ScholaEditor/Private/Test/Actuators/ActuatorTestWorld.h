// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once

#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#include "Tests/AutomationEditorCommon.h"

/** Shared flags for Schola actuator editor automation tests (inline for unity builds). */
inline constexpr EAutomationTestFlags GScholaActuatorEditorTestFlags =
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter;

/**
 * RAII wrapper around FTestWorldWrapper for actuator tests: creates a dedicated game world,
 * runs BeginPlay, ticks a few frames, then EndPlay + Destroy on scope exit.
 */
struct FScholaActuatorTestWorld
{
	enum class EPhase : uint8
	{
		None,
		Created,
		Playing,
	};

	FTestWorldWrapper Wrapper;
	FAutomationTestBase* Test = nullptr;
	EPhase Phase = EPhase::None;

	bool Setup(FAutomationTestBase* InTest)
	{
		Test = InTest;
		if (!Wrapper.CreateTestWorld(EWorldType::Game))
		{
			Test->AddError(TEXT("Failed to create actuator test world"));
			return false;
		}
		Phase = EPhase::Created;

		if (!Wrapper.GetTestWorld())
		{
			Test->AddError(TEXT("Failed to get actuator test world"));
			Wrapper.DestroyTestWorld(true);
			Phase = EPhase::None;
			return false;
		}

		if (!Wrapper.BeginPlayInTestWorld())
		{
			Test->AddError(TEXT("Failed to begin play in actuator test world"));
			Wrapper.DestroyTestWorld(true);
			Phase = EPhase::None;
			return false;
		}

		Phase = EPhase::Playing;
		for (int32 i = 0; i < 3; ++i)
		{
			Wrapper.TickTestWorld(0.016f);
		}
		return true;
	}

	void Tick(float DeltaSeconds = 0.016f)
	{
		if (Phase == EPhase::Playing)
		{
			Wrapper.TickTestWorld(DeltaSeconds);
		}
	}

	UWorld* GetWorld() const { return Wrapper.GetTestWorld(); }

	~FScholaActuatorTestWorld()
	{
		if (Phase == EPhase::Playing)
		{
			Wrapper.EndPlayInTestWorld();
		}
		if (Phase == EPhase::Playing || Phase == EPhase::Created)
		{
			Wrapper.DestroyTestWorld(true);
		}
	}

	FScholaActuatorTestWorld() = default;
	FScholaActuatorTestWorld(const FScholaActuatorTestWorld&) = delete;
	FScholaActuatorTestWorld& operator=(const FScholaActuatorTestWorld&) = delete;
};
