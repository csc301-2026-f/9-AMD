// Copyright (c) 2023 Advanced Micro Devices, Inc. All Rights Reserved.

#include "GymConnectors/ExternalGymConnector.h"
#include "LogScholaTraining.h"

UExternalGymConnector::UExternalGymConnector()
{
}


FTrainingStateUpdate* UExternalGymConnector::ResolveEnvironmentStateUpdate()
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("ScholaTraining: ExternalGymConnector Resolve Environment State Update");
	UE_LOGFMT(LogScholaTraining, Verbose, "UExternalGymConnector::ResolveEnvironmentStateUpdate(): Resolving State Update");
	TFuture<FTrainingStateUpdate*> UpdateFuture = this->RequestStateUpdate();
	if(this->ExternalSettings.bUseTimeout)
	{
		if(UpdateFuture.WaitFor(FTimespan(0, 0, this->ExternalSettings.Timeout)))
		{	
			return UpdateFuture.Get();
		}
		else
		{
			UE_LOGFMT(LogScholaTraining, Warning, "UExternalGymConnector::ResolveEnvironmentStateUpdate(): Gym Connector timed out. Marking as errored");
			this->Status = EConnectorStatus::Error;
			UpdateFuture.Reset();
			return nullptr;
		}
	}
	else //No Timeout so wait forever, can freeze Unreal Engine
	{
		UpdateFuture.Wait();
		return UpdateFuture.Get();
	}
	
}
