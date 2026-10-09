// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "Adapters/BoxStacker.h"
#include "Containers/ArrayView.h"
#include "Points/BoxPoint.h"
#include "Spaces/BoxSpace.h"
#include "Common/LogSchola.h"

namespace
{
	int32 GetProduct(TConstArrayView<int> Shape)
	{
		if (Shape.Num() == 0) { return 0; }
		int32 Product = 1;
		for (int32 Dim : Shape)
		{
			Product *= Dim;
		}
		return Product;
	}

	TArray<int> InsertDimensionAt(TConstArrayView<int> UnstackedShape, int32 InsertIndex, int32 StackSize)
	{
		TArray<int> StackedShape;
		const int32 Rank = UnstackedShape.Num();
		const int32 K = FMath::Clamp(InsertIndex, 0, Rank);
		for (int32 i = 0; i < K; ++i)
		{
			StackedShape.Add(UnstackedShape[i]);
		}
		StackedShape.Add(StackSize);
		for (int32 i = K; i < Rank; ++i)
		{
			StackedShape.Add(UnstackedShape[i]);
		}
		return StackedShape;
	}

	TArray<int> ExpandDimensionAt(TConstArrayView<int> UnstackedShape, int32 ExpandIndex, int32 StackSize)
	{
		TArray<int> StackedShape = TArray<int>(UnstackedShape);
		const int32 Rank = StackedShape.Num();
		if (Rank > 0)
		{
			const int32 K = FMath::Clamp(ExpandIndex, 0, Rank - 1);
			StackedShape[K] *= StackSize;
		}
		return StackedShape;
	}
}

void UBoxStacker::ValidateDefaultPointAndSpace()
{
	Super::ValidateDefaultPointAndSpace();
	checkf(UnstackedSpace.GetPtr<FBoxSpace>(), TEXT("UBoxStacker::ValidateDefaultPointAndSpace(): UnstackedSpace is not a Box Space."));
}

void UBoxStacker::Push(const FInstancedStruct& InPoint, FInstancedStruct& OutStackedPoint)
{
	if(InPoint.GetPtr<FBoxPoint>() == nullptr)
	{
		UE_LOGFMT(LogSchola, Error, "UBoxStacker::Push(): Box Stacker only supports FBoxPoint - Ignoring.");
		GetStacked(OutStackedPoint);
		return;
	}
	Super::Push(InPoint, OutStackedPoint);
}


void UBoxStacker::GetStacked(FInstancedStruct& OutStackedPoint)
{
	OutStackedPoint.InitializeAs<FBoxPoint>();
	GetStacked(OutStackedPoint.GetMutable<FBoxPoint>());
}

void UBoxStacker::GetStacked(FBoxPoint& OutStackedPoint)
{
	if (Buffer.Num() == 0)
	{
		PopulateBufferWithDefaults();
	}

	const int32 N = FMath::Max(1, StackSize);

	// Resolve unstacked shape: from UnstackedSpace
	TArray<int> UnstackedShape;
	int32 FrameSize = 0;

	
	if (const FBoxSpace* BoxSpace = UnstackedSpace.GetPtr<FBoxSpace>())
	{
		if (BoxSpace->Shape.Num() > 0)
		{
			UnstackedShape = BoxSpace->Shape;
		}
		else
		{
			UnstackedShape = { BoxSpace->Dimensions.Num() };
		}
		FrameSize = GetProduct(UnstackedShape);
	}

	if (FrameSize == 0)
	{
		OutStackedPoint.Values.Reset();
		OutStackedPoint.Shape.Reset();
		return;
	}
	
	const int32 Rank = UnstackedShape.Num();
	// We can add a new dimension at position Rank, but can only expand existing dimensions up to Rank-1, since we need a dimension to expand.	
	const int32	MaxK = bAddNewDimension ? Rank : Rank - 1;
	const int32 K = FMath::Clamp(StackDimensionIndex, 0, MaxK);

	const TArray<int> StackedShape = bAddNewDimension
		? InsertDimensionAt(UnstackedShape, K, N)
		: ExpandDimensionAt(UnstackedShape, K, N);
	const int32 TotalSize = N * FrameSize;

	OutStackedPoint.Values.SetNumUninitialized(TotalSize);
	OutStackedPoint.Shape = StackedShape;

	const TArray<int>	 UnitaryShapeArray = { 1 };
	TConstArrayView<int> ContiguousFrameShape;
	TConstArrayView<int> NonContiguousFrameShape;
	// Contiguous block size and count depend on insert vs expand.
	// Insert: new dim at K, so contiguous part in output is StackedShape[K:Rank].
	// Expand: dim K is expanded, so contiguous part in output is StackedShape[K+1:Rank].
	if(K == Rank)
	{
		//Special case of adding a new dimension at the end of the shape. The whole shape is non-contiguous.
		ContiguousFrameShape = MakeArrayView(UnitaryShapeArray);
		NonContiguousFrameShape = MakeArrayView(UnstackedShape);
	}
	else
	{
		const int32 ContiguousRank = Rank - K;
		ContiguousFrameShape = MakeArrayView(UnstackedShape.GetData() + K, ContiguousRank);
		NonContiguousFrameShape = MakeArrayView(UnstackedShape.GetData(), K);
	}
	const int32 ContiguousFrameSize = FMath::Max(1, GetProduct(ContiguousFrameShape));
	const int32 NonContiguousFrames = FMath::Max(1, GetProduct(NonContiguousFrameShape));
	
	// Non-contiguous: dims before the stack + stack size N. Expand also includes dim K in the "block" count.
	// Copy each block to the correct location. For shape [2,2,2] stacking 2 on dim 1, output is [2,4,2]:
	// repeat blocks of size [2,2] (ContiguousFrameSize=4) [2,2] times (2 from dim0 × 2 from stack).
	for (int32 b = 0; b < NonContiguousFrames; ++b)
	{
		for (int32 PointIndex = 0; PointIndex < N; ++PointIndex)
		{
			const TInstancedStruct<FPoint>& Point = Buffer[PointIndex]; 
			const FBoxPoint* Source = Point.GetPtr<FBoxPoint>();
			//The offset into the output point is the block index * the number of Stacked Points + The current point we are stacking
			const int32 DstOffset = (b*N + PointIndex) * ContiguousFrameSize;
			const int32 SrcOffset = b * ContiguousFrameSize;
			
			// After Init we always have a valid source point, so we can just memcpy the values.
			FMemory::Memcpy(&OutStackedPoint.Values[DstOffset], &Source->Values[SrcOffset], ContiguousFrameSize * sizeof(float));
		}
	}
}

void UBoxStacker::GetStackedSpace(FInstancedStruct& OutSpace) const
{
	OutSpace.InitializeAs<FBoxSpace>();
	GetStackedSpace(OutSpace.GetMutable<FBoxSpace>());
}

void UBoxStacker::GetStackedSpace(FBoxSpace& OutSpace) const
{
	OutSpace.Dimensions.Reset();
	OutSpace.Shape.Reset();

	if (!UnstackedSpace.IsValid()) { return; }

	const FBoxSpace* UnstackedBox = UnstackedSpace.GetPtr<FBoxSpace>();
	if (!UnstackedBox) { return; }

	TArray<int> UnstackedShape;
	if (UnstackedBox->Shape.Num() > 0)
	{
		UnstackedShape = UnstackedBox->Shape;
	}
	else
	{
		UnstackedShape = { UnstackedBox->Dimensions.Num() };
	}

	const int32 N = FMath::Max(1, StackSize);
	const int32 Rank = UnstackedShape.Num();
	const int32	MaxK = bAddNewDimension ? Rank : Rank - 1;
	const int32 K = FMath::Clamp(StackDimensionIndex, 0, MaxK);
	TConstArrayView<int> UnstackedShapeView = MakeArrayView(UnstackedShape);
	const TArray<int> StackedShape = bAddNewDimension
		   ? InsertDimensionAt(UnstackedShapeView, K, N)
		: ExpandDimensionAt(UnstackedShapeView, K, N);
	const int32	FrameSize = GetProduct(UnstackedShapeView);

	OutSpace.Shape = StackedShape;
	OutSpace.Dimensions.Reserve(N * FrameSize);

	// Replicate dimensions for each frame (row-major order)
	for (int32 i = 0; i < N; ++i)
	{
		for (int32 j = 0; j < FrameSize; ++j)
		{
			OutSpace.Dimensions.Add(UnstackedBox->Dimensions[j]);
		}
	}

	TConstArrayView<int> ContiguousFrameShape;
	TConstArrayView<int> NonContiguousFrameShape;
	const TArray<int>	 UnitaryShapeArray = { 1 };
	// Contiguous block size and count depend on insert vs expand.
	// Insert: new dim at K, so contiguous part in output is StackedShape[K:Rank].
	// Expand: dim K is expanded, so contiguous part in output is StackedShape[K+1:Rank].
	if(K == Rank)
	{
		//Special case of adding a new dimension at the end of the shape. The whole shape is non-contiguous.
		ContiguousFrameShape = MakeArrayView(UnitaryShapeArray);
		NonContiguousFrameShape = MakeArrayView(UnstackedShape);
	}
	else
	{
		const int32 ContiguousRank = Rank - K;
		ContiguousFrameShape = MakeArrayView(UnstackedShape.GetData() + K, ContiguousRank);
		NonContiguousFrameShape = MakeArrayView(UnstackedShape.GetData(), K);
	}
	const int32 ContiguousFrameSize = FMath::Max(1, GetProduct(ContiguousFrameShape));
	const int32 NonContiguousFrames = FMath::Max(1, GetProduct(NonContiguousFrameShape));
	
	// Non-contiguous: dims before the stack + stack size N. Expand also includes dim K in the "block" count.
	// Copy each block to the correct location. For shape [2,2,2] stacking 2 on dim 1, output is [2,4,2]:
	// repeat blocks of size [2,2] (ContiguousFrameSize=4) [2,2] times (2 from dim0 × 2 from stack).
	for (int32 b = 0; b < NonContiguousFrames; ++b)
	{
		for (int32 StackIndex = 0; StackIndex < N; ++StackIndex)
		{
			//The offset into the output space is the block index * the number of Stacked Points + The current point we are stacking
			const int32 DstOffset = (b * N + StackIndex) * ContiguousFrameSize;
			const int32 SrcOffset = b * ContiguousFrameSize;

			// Copy the Space Dimensions for the current block
			FMemory::Memcpy(&OutSpace.Dimensions[DstOffset], &UnstackedBox->Dimensions[SrcOffset], ContiguousFrameSize * sizeof(FBoxSpaceDimension));
		}
	}

}
