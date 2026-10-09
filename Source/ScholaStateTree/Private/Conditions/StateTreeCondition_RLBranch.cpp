// Copyright (c) 2026 Advanced Micro Devices, Inc. All Rights Reserved.

#include "Conditions/StateTreeCondition_RLBranch.h"
#include "StateTreeExecutionContext.h"
#include "LogScholaStateTree.h"

bool FStateTreeCondition_RLBranch::TestCondition(FStateTreeExecutionContext& Context) const
{
	// Get instance data which holds the bound inputs
	const FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	// Check confidence threshold first
	if (MinConfidence > 0.0f && InstanceData.Confidence < MinConfidence)
	{
		UE_LOG(LogScholaStateTree, Verbose,
			TEXT("RLBranch: Confidence %.3f below threshold %.3f"),
			InstanceData.Confidence, MinConfidence);
		return bInvertCondition; // Return inverted false
	}

	// Check if branch matches
	bool bBranchMatches = (InstanceData.SelectedBranch == BranchIndex);

	return bInvertCondition ? !bBranchMatches : bBranchMatches;
}
