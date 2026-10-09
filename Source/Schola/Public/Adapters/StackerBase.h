// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "StructUtils/InstancedStruct.h"
#include "Points/Point.h"
#include "Spaces/Space.h"
#include "Containers/RingBuffer.h"

#include "StackerBase.generated.h"

/**
 * @class UStackerBase
 * @brief Abstract base for stackers that hold a ring buffer of points and expose stacked point/space.
 *
 * Subclasses (e.g. UDictStacker, UBoxStacker) implement GetStacked and GetStackedSpace for their concrete types.
 */
UCLASS(Abstract, BlueprintType, EditInlineNew, DefaultToInstanced)
class SCHOLA_API UStackerBase : public UObject
{
	GENERATED_BODY()

public:
	/** The unstacked space that will be stacked */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = "Schola|Stacker")
	TInstancedStruct<FSpace> UnstackedSpace;

	/** Number of frames to keep in the ring buffer (stack size). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Schola|Stacker", meta = (ClampMin = "1", ExposeOnSpawn = true))
	int32 StackSize = 4;

	/** The default point to represent empty slots in the stacked observation. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Schola|Stacker", meta = (ExposeOnSpawn = true))
	TInstancedStruct<FPoint> DefaultPoint;

	/**
	 * Pushes a point into the ring buffer (overwrites oldest when full). After updating the buffer, fills OutStackedPoint via GetStacked.
	 * @param InPoint The input point to stack.
	 * @param OutStackedPoint Output stacked point (type is defined by the subclass).
	 */
	UFUNCTION(BlueprintCallable, Category = "Schola|Stacker")
	virtual void Push(const FInstancedStruct& InPoint, FInstancedStruct& OutStackedPoint);

	/**
	 * Resets the ring buffer; next push will start at index 0 again.
	 */
	UFUNCTION(BlueprintCallable, Category = "Schola|Stacker")
	void Reset();

	/** @return Number of real (non-default) entries currently in the stack (0..StackSize). */
	UFUNCTION(BlueprintPure, Category = "Schola|Stacker")
	int32 GetNumValid() const { return NumValid_; }

	/**
	 * Fills the output with the current stacked point. Index 0 = newest, highest index = oldest.
	 * @param OutStackedPoint Output stacked point (subclass-specific type wrapped in FInstancedStruct).
	 */
	UFUNCTION(BlueprintCallable, Category = "Schola|Stacker")
	virtual void GetStacked(FInstancedStruct& OutStackedPoint) PURE_VIRTUAL(UStackerBase::GetStacked, );

	/**
	 * Fills the output with the stacked space.
	 * @param OutSpace Output stacked space (subclass-specific type wrapped in FInstancedStruct).
	 */
	UFUNCTION(BlueprintCallable, Category = "Schola|Stacker")
	virtual void GetStackedSpace(FInstancedStruct& OutSpace) const PURE_VIRTUAL(UStackerBase::GetStackedSpace, );

protected:
	/** Ring buffer of the last StackSize points (newest at logical push position). */
	TRingBuffer<TInstancedStruct<FPoint>> Buffer;
	/** Number of real (pushed) entries in the buffer; remaining slots are DefaultPoint. */
	int32 NumValid_ = 0;

	/** Fills unused slots in Buffer with DefaultPoint after resize or reset. */
	void PopulateBufferWithDefaults();
	/** Ensures DefaultPoint and UnstackedSpace are compatible for this stacker; assert or log on failure. */
	virtual void ValidateDefaultPointAndSpace();
};
