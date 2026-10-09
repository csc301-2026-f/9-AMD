// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once

#include "Adapters/StackerBase.h"
#include "Points/DictPoint.h"
#include "Spaces/DictSpace.h"

#include "DictStacker.generated.h"

/**
 * @class UDictStacker
 * @brief UObject that stacks dictionary inputs in a ring buffer and exposes them as one dictionary.
 *
 * Each push stores one dict (e.g. one observation). The stacked output uses keys
 * "OriginalKey_0", "OriginalKey_1", ... where index 0 = newest, highest index = oldest.
 */
UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class SCHOLA_API UDictStacker : public UStackerBase
{
	GENERATED_BODY()

public:
	/**
	 * Fills the output with the stacked dictionary: keys are "OriginalKey_0", "OriginalKey_1", ...
	 * Index 0 = newest, highest index = oldest. Clears OutStackedPoint and overwrites it with the current stacked state.
	 * @param OutStackedPoint Output DictPoint of stacked keys to point values.
	 */
	void GetStacked(FInstancedStruct& OutStackedPoint) override;

	/**
	 * Fills the output with the stacked dictionary: keys are "OriginalKey_0", "OriginalKey_1", ...
	 * Index 0 = newest, highest index = oldest.
	 * @param OutStackedPoint Output DictPoint of stacked keys to point values.
	 */
	void GetStacked(FDictPoint& OutStackedPoint);

	/**
	 * Returns a DictSpace containing the stacked points.
	 * @param OutSpace Output DictSpace of stacked keys to point values.
	 */
	
	void GetStackedSpace(FInstancedStruct& OutSpace) const override;

	/**
	 * Returns a DictSpace containing the stacked Space.
	 * @param OutSpace Output DictSpace of stacked keys to point values.
	 */
	void GetStackedSpace(FDictSpace& OutSpace) const;
};
