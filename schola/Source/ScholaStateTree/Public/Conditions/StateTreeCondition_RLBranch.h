// Copyright (c) 2026 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeConditionBase.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeCondition_RLBranch.generated.h"

/**
 * @brief Instance data for FStateTreeCondition_RLBranch.
 *
 * Properties in "Input" category are bindable from evaluator outputs.
 * Properties in "Parameter" category are configured in the editor.
 */
USTRUCT()
struct SCHOLASTATETREE_API FStateTreeCondition_RLBranchInstanceData
{
	GENERATED_BODY()

	/**
	 * The selected branch from an RL Decision Evaluator.
	 * Bind this to the evaluator's SelectedBranch output.
	 */
	UPROPERTY(EditAnywhere, Category = "Input")
	int32 SelectedBranch = -1;

	/**
	 * The confidence from an RL Decision Evaluator.
	 * Optional - defaults to 1.0 if not bound.
	 */
	UPROPERTY(EditAnywhere, Category = "Input", meta = (Optional))
	float Confidence = 1.0f;
};

/**
 * @brief State Tree Condition that checks if the RL Decision Evaluator selected a specific branch.
 *
 * This condition is designed to work with FStateTreeEvaluator_RLDecision. It reads the
 * SelectedBranch value from a linked evaluator's instance data and compares it against
 * a configured branch index.
 *
 * ## Usage:
 * 1. Add an FStateTreeEvaluator_RLDecision to your State Tree
 * 2. Create transitions from a parent state to child states
 * 3. Add FStateTreeCondition_RLBranch to each transition
 * 4. Configure BranchIndex (0, 1, 2, etc.) to match the RL policy's output
 * 5. Bind SelectedBranch input to the evaluator's SelectedBranch output
 *
 * ## Example State Tree Structure:
 * ```
 * Root State
 *   ├── Evaluator: RLDecision (outputs SelectedBranch 0-2)
 *   ├── Transition → Patrol State   [Condition: RLBranch == 0]
 *   ├── Transition → Attack State   [Condition: RLBranch == 1]
 *   └── Transition → Idle State     [Condition: RLBranch == 2]
 * ```
 *
 * @see FStateTreeEvaluator_RLDecision
 */
USTRUCT(meta = (DisplayName = "RL Branch Check", Category = "Schola|Conditions"))
struct SCHOLASTATETREE_API FStateTreeCondition_RLBranch : public FStateTreeConditionCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeCondition_RLBranchInstanceData;

	FStateTreeCondition_RLBranch() = default;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	/**
	 * The branch index to check against.
	 * This should match one of the possible outputs from the RL policy (0, 1, 2, ...).
	 * The condition passes if SelectedBranch == BranchIndex.
	 */
	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (ClampMin = "0"))
	int32 BranchIndex = 0;

	/**
	 * If true, the condition passes when the branch does NOT match.
	 * Useful for "else" or "default" transitions.
	 */
	UPROPERTY(EditAnywhere, Category = "Parameter")
	bool bInvertCondition = false;

	/**
	 * Minimum confidence threshold for the condition to pass.
	 * If the RL evaluator's confidence is below this value, the condition fails
	 * regardless of the branch match. Set to 0 to disable confidence checking.
	 * Useful for implementing fallback logic when the policy is uncertain.
	 */
	UPROPERTY(EditAnywhere, Category = "Parameter", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MinConfidence = 0.0f;

protected:
	virtual bool TestCondition(FStateTreeExecutionContext& Context) const override;
};
