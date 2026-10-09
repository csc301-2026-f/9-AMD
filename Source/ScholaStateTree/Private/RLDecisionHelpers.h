// Copyright (c) 2026 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"
#include "Points/Point.h"
#include "Spaces/Space.h"

/**
 * @brief Helper functions for RL Decision evaluators.
 */
namespace RLDecisionHelpers
{
	/**
	 * Extracts the discrete branch index from an action buffer.
	 * Handles FDiscretePoint, FDictPoint (first discrete entry), and FBoxPoint (argmax).
	 *
	 * @param ActionBuffer The action output from policy inference
	 * @param InOutNumBranches Updated with branch count (for BoxPoint case)
	 * @param OutBranchScores Filled with scores for each branch
	 * @param OutConfidence Set to the confidence of the selection
	 * @return The selected branch index, or -1 if extraction failed
	 */
	int32 ExtractBranchIndexFromAction(
		const TInstancedStruct<FPoint>& ActionBuffer,
		int32&							InOutNumBranches,
		TArray<float>&					OutBranchScores,
		float&							OutConfidence);

	/**
	 * Determines the number of branches from an action space definition.
	 *
	 * @param ActionSpaceDefn The action space definition
	 * @return The number of possible branches/actions
	 */
	int32 GetNumBranchesFromActionSpace(const TInstancedStruct<FSpace>& ActionSpaceDefn);

} // namespace RLDecisionHelpers
