// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"
//#include "Tests/AutomationCommon.h"
#include "CoreMinimal.h"
#include "TestStepper.h"
#include "Spaces/BoxSpace.h"
#include "Spaces/MultiDiscreteSpace.h"
#include "Steppers/PipelinedStepper.h"
#include "UObject/StrongObjectPtr.h"

/**
 * Latent test context for pipelined stepper.
 */
struct FPipelinedStepperTestContext
{
	// Strong references so objects survive GC across latent frames without manual AddToRoot.
	TStrongObjectPtr<UTestAgent> Agent;
	TStrongObjectPtr<UTestPolicy> Policy;
	TStrongObjectPtr<UPipelinedStepper> Stepper;

	int StepsRemaining = 0;
	int TotalSteps = 0;
	FAutomationTestBase* Test = nullptr;
	bool bInitialized = false;
};

// Create objects & initialize stepper
DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(FCreatePipelinedStepperObjectsCommand, TSharedPtr<FPipelinedStepperTestContext>, Context);
bool FCreatePipelinedStepperObjectsCommand::Update()
{
	if (!Context.IsValid()) return true;
	if (Context->bInitialized) return true;

	FInteractionDefinition Def;
	Context->Agent = TStrongObjectPtr<UTestAgent>(NewObject<UTestAgent>());
	Context->Policy = TStrongObjectPtr<UTestPolicy>(NewObject<UTestPolicy>());
	Context->Stepper = TStrongObjectPtr<UPipelinedStepper>(NewObject<UPipelinedStepper>());

	IAgent::Execute_Define(Context->Agent.Get(), Def);

	TArray<TScriptInterface<IAgent>> Agents;

	Agents.Add(Context->Agent.Get());
	TScriptInterface<IPolicy> PolicyInterface = Context->Policy.Get();

	Context->bInitialized = Context->Stepper->Init(Agents, PolicyInterface);
	if (Context->Test)
	{
		Context->Test->TestTrue(TEXT("PipelinedStepper initialized"), Context->bInitialized);
	}
	return true; // done
}

// Perform one Step per frame until StepsRemaining exhausted
DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(FStepPipelinedStepperLatentCommand, TSharedPtr<FPipelinedStepperTestContext>, Context);
bool FStepPipelinedStepperLatentCommand::Update()
{
	if (!Context.IsValid() || !Context->bInitialized)
	{
		return true; // abort
	}
	if (Context->StepsRemaining <= 0)
	{
		return true; // finished stepping
	}

	if (Context->Stepper)
	{
		Context->Stepper->Step();
		--Context->StepsRemaining;
		++Context->TotalSteps;
	}
	// Return false to continue next frame if more steps remain
	return Context->StepsRemaining <= 0;
}

// Verify that an action was eventually applied (LastActionReceived == 1)
DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(FVerifyPipelinedStepperResultCommand, TSharedPtr<FPipelinedStepperTestContext>, Context);
bool FVerifyPipelinedStepperResultCommand::Update()
{
	if (!Context.IsValid() || !Context->Agent)
	{
		return true;
	}
	if (!Context->bInitialized)
	{
		if (Context->Test)
		{
			Context->Test->AddError(TEXT("Initialization failed; skipping action verification."));
		}
		return true;
	}
	int LastAction = Context->Agent->GetLastActionReceived();
	if (Context->Test)
	{
		// Context->Test->TestTrue(TEXT("Agent received a valid action"), LastAction >= 0);
		Context->Test->TestEqual(TEXT("Agent received last action 1"), LastAction, 1);
		// Thread usage assertions (Policy must have run off game thread at least once)
		const TSet<uint32> ThreadIds = UTestPolicy::GetThreadIdsCopy();
		const bool bSawNonGame = UTestPolicy::SawNonGameThread();
		Context->Test->TestTrue(TEXT("Policy executed on a non-game thread"), bSawNonGame);
		Context->Test->TestTrue(TEXT("Policy Think ran at least once"), ThreadIds.Num() > 0);
		Context->Test->AddInfo(FString::Printf(TEXT("Total Steps Executed: %d, LastAction=%d"), Context->TotalSteps, LastAction));
		Context->Test->AddInfo(FString::Printf(TEXT("Policy Thread Count=%d NonGame=%s"), ThreadIds.Num(), bSawNonGame ? TEXT("True") : TEXT("False")));
	}
	return true;
}

// (Add a cleanup latent command)
DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(FCleanupPipelinedStepperCommand, TSharedPtr<FPipelinedStepperTestContext>, Context);
bool FCleanupPipelinedStepperCommand::Update()
{
    if (!Context.IsValid()) return true;

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FPipelinedStepperTest, "Schola.Steppers.PipelinedStepper Latent", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)

bool FPipelinedStepperTest::RunTest(const FString& Parameters)
{
    TSharedPtr<FPipelinedStepperTestContext> Context = MakeShareable(new FPipelinedStepperTestContext());
    Context->StepsRemaining = 6; // Needs >=2 for pipeline (collect/dispatch then apply)
    Context->TotalSteps = 0;
    Context->Test = this;

	// Reset thread tracking before starting
	UTestPolicy::ResetThreadTracking();

    ADD_LATENT_AUTOMATION_COMMAND(FCreatePipelinedStepperObjectsCommand(Context));
    //ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(0.01f));
    ADD_LATENT_AUTOMATION_COMMAND(FStepPipelinedStepperLatentCommand(Context));
    ADD_LATENT_AUTOMATION_COMMAND(FVerifyPipelinedStepperResultCommand(Context));
    ADD_LATENT_AUTOMATION_COMMAND(FCleanupPipelinedStepperCommand(Context));

    return true;
}