// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once

#include "Adapters/StackerBase.h"
#include "Points/BoxPoint.h"
#include "Spaces/BoxSpace.h"

#include "BoxStacker.generated.h"

/**
 * @class UBoxStacker
 * @brief UObject that stacks BoxPoint inputs in a ring buffer and exposes them as one BoxPoint.
 *
 * Each push stores one BoxPoint (e.g. one observation frame). The stacked output inserts or expands
 * a dimension at StackDimensionIndex; bAddNewDimension selects insert vs expand. Index 0 = newest,
 * highest index = oldest. Memory layout is row-major (C-order).
 */
UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class SCHOLA_API UBoxStacker : public UStackerBase
{
	GENERATED_BODY()

public:
	/**
	 * Index at which the stack dimension is inserted or expanded (0-based).
	 * When bAddNewDimension is true: a new dimension of size StackSize is inserted at this index (e.g. 0 -> (N,a,b,...)).
	 * When bAddNewDimension is false: the dimension at this index is expanded by StackSize (e.g. (a,b) with index 1 and StackSize N -> (a, b * N)).
	 * Clamped at runtime to [0, rank of unstacked shape].
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Schola|Stacker", meta = (ClampMin = "0", ExposeOnSpawn = true))
	int32 StackDimensionIndex = 0;

	/** If true, insert a new dimension of size StackSize at StackDimensionIndex. If false, expand the dimension at StackDimensionIndex by StackSize. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Schola|Stacker", meta = (ExposeOnSpawn = true))
	bool bAddNewDimension = true;

	/**
	 * Fills the output with the stacked BoxPoint. Index 0 = newest, highest index = oldest.
	 * @param OutStackedPoint Output BoxPoint of stacked values.
	 */
	void GetStacked(FInstancedStruct& OutStackedPoint) override;

	/**
	 * Fills the output with the stacked BoxPoint. Index 0 = newest, highest index = oldest.
	 * @param OutStackedPoint Output BoxPoint of stacked values.
	 */
	void GetStacked(FBoxPoint& OutStackedPoint);

	void Push(const FInstancedStruct& InPoint, FInstancedStruct& OutStackedPoint) override;

	/**
	 * Returns a BoxSpace containing the stacked space.
	 * @param OutSpace Output BoxSpace with stacked shape.
	 */
	void GetStackedSpace(FInstancedStruct& OutSpace) const override;

	/**
	 * Returns a BoxSpace containing the stacked space.
	 * @param OutSpace Output BoxSpace with stacked shape.
	 */
	void GetStackedSpace(FBoxSpace& OutSpace) const;
protected:
	/** Validates that default point/space types match box stacking expectations. */
	virtual void ValidateDefaultPointAndSpace() override;
};
