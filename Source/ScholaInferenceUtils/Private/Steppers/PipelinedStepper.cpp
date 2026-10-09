// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "Steppers/PipelinedStepper.h"
#include "Async/Async.h"
#include "LogScholaInferenceUtils.h"

void UPipelinedStepper::Step()
{
    if (!Policy || Agents.Num() == 0)
    {
        UE_LOGFMT(LogScholaInferenceUtils, Error, "PipelinedStepper::Step(): Invalid state - missing policy or agents");
        return;
    }

    const int32 CurrentFrame = TickCounter % PIPELINE_STAGES;
    const bool bHavePrevious = TickCounter > 0;
    const int32 PrevFrame = bHavePrevious ? (TickCounter - 1) % PIPELINE_STAGES : -1;

	if (TickCounter > 0 && Frames[PrevFrame].bActionsReady)
	{
		UE_LOGFMT(LogScholaInferenceUtils, Verbose, "PipelinedStepper::Step(): PrevFrame actions ready - DispatchId={0} TickCounter={1} ThreadId={2}",
			static_cast<uint64>(Frames[PrevFrame].DebugDispatchId),
			static_cast<uint64>(TickCounter),
			FPlatformTLS::GetCurrentThreadId());

        auto& Frame = Frames[PrevFrame];

        if (Frame.Actions.Num() != Agents.Num())
        {
            UE_LOGFMT(LogScholaInferenceUtils, Error, "PipelinedStepper::Step(): Action count mismatch - {0} actions for {1} agents", Frame.Actions.Num(), Agents.Num());
        }
        else
        {
            for (int i = 0; i < Agents.Num(); ++i)
            {
                IAgent::Execute_Act(Agents[i].GetObject(),Frame.Actions[i]);
            }
        }

        Frame.Actions.Reset();
        Frame.bActionsReady = false;
    }
    
    auto& Frame = Frames[CurrentFrame];
    Frame.Observations.Reset();
    Frame.Actions.Reset();
    Frame.bActionsReady = false;
    Frame.bThinkInFlight = true;

    Frame.Observations.Reserve(Agents.Num());

    for (int i = 0; i < Agents.Num(); ++i)
    {
        TInstancedStruct<FPoint> Obs;
        IAgent::Execute_Observe(Agents[i].GetObject(),Obs);
        Frame.Observations.Add(Obs);
    }
    
    if (Policy->IsInferenceBusy())
    {
		return;
    }
    DispatchThink(CurrentFrame);
    
    ++TickCounter;
}

void UPipelinedStepper::DispatchThink(int32 FrameIndex)
{
    FPipelinedStepperFrame* FramePtr = &Frames[FrameIndex];
    TArray<TInstancedStruct<FPoint>> ObservationsCopy = FramePtr->Observations;

    TWeakObjectPtr<UPipelinedStepper> WeakThis(this);
    TScriptInterface<IPolicy> PolicyLocal = Policy;

    // Reserve a unique dispatch id on the Game Thread
    const uint64 DispatchId = DebugDispatchSeq.fetch_add(1, std::memory_order_relaxed) + 1;
    FramePtr->DebugDispatchId = DispatchId;
    UE_LOGFMT(LogScholaInferenceUtils, Verbose, "PipelinedStepper::DispatchThink(): Scheduled - DispatchId={0} FrameIndex={1} ThreadId={2}",
        DispatchId, FrameIndex, FPlatformTLS::GetCurrentThreadId());

    Async(EAsyncExecution::ThreadPool, [WeakThis, FrameIndex, Observations = MoveTemp(ObservationsCopy), PolicyLocal, DispatchId]() {
        if (!WeakThis.IsValid() || !PolicyLocal)
            return;

        UE_LOGFMT(LogScholaInferenceUtils, Verbose, "PipelinedStepper::DispatchThink(): Think start - DispatchId={0} FrameIndex={1} ThreadId={2}",
            DispatchId, FrameIndex, FPlatformTLS::GetCurrentThreadId());

        TArray<TInstancedStruct<FPoint>> ActionsLocal;
        const bool bSuccess = PolicyLocal->BatchedThink(const_cast<TArray<TInstancedStruct<FPoint>>&>(Observations), ActionsLocal);

        AsyncTask(ENamedThreads::GameThread, [WeakThis, FrameIndex, bSuccess, Actions = MoveTemp(ActionsLocal), DispatchId]() mutable {
            if (!WeakThis.IsValid())
                return;
            if (WeakThis->bShuttingDown)
                return;

            auto& Frame = WeakThis->Frames[FrameIndex];
            if (!bSuccess)
            {
                UE_LOGFMT(LogScholaInferenceUtils, Error, "PipelinedStepper::DispatchThink(): Think failed - DispatchId={0} FrameIndex={1}",
                    DispatchId, FrameIndex);
                Frame.bThinkInFlight = false;
                return;
            }
            Frame.Actions = MoveTemp(Actions);
            Frame.bActionsReady = true;
            Frame.bThinkInFlight = false;

            UE_LOGFMT(LogScholaInferenceUtils, Verbose, "PipelinedStepper::DispatchThink(): Think complete - DispatchId={0} FrameIndex={1} ThreadId={2}",
                DispatchId, FrameIndex, FPlatformTLS::GetCurrentThreadId());
        });
    });
}
