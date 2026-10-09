// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "Adapters/DictStacker.h"
#include "Points/DictPoint.h"
#include "Spaces/DictSpace.h"

void UDictStacker::GetStacked(FInstancedStruct& OutStackedPoint)
{
	OutStackedPoint.InitializeAs<FDictPoint>();
	GetStacked(OutStackedPoint.GetMutable<FDictPoint>());
}

void UDictStacker::GetStacked(FDictPoint& OutStackedPoint)
{
	if (Buffer.Num() == 0)
	{
		PopulateBufferWithDefaults();
	}
	OutStackedPoint.Points.Reset();
	const int32 N = FMath::Max(1, StackSize);
	// Index 0 = newest (back of ring), highest index = oldest.
	for (int32 i = 0; i < N; ++i)
	{
		const TInstancedStruct<FPoint>& Point = Buffer[i];
		const FString Suffix = FString::Printf(TEXT("_%d"), i);
		if (const FDictPoint* Dict = Point.GetPtr<FDictPoint>())
		{
			for (const auto& Pair : Dict->Points)
			{
				OutStackedPoint.Points.Add(Pair.Key + Suffix, Pair.Value);
			}
		}
		else
		{
			OutStackedPoint.Points.Add(FString::Printf(TEXT("Point_%d"), i), Point);
		}
	}
}

void UDictStacker::GetStackedSpace(FInstancedStruct& OutSpace) const
{
	OutSpace.InitializeAs<FDictSpace>();
	GetStackedSpace(OutSpace.GetMutable<FDictSpace>());
}

void UDictStacker::GetStackedSpace(FDictSpace& OutSpace) const
{
	OutSpace.Spaces.Reset();
	if (!UnstackedSpace.IsValid()) { return; }

	const int32 N = FMath::Max(1, StackSize);
	if (const FDictSpace* UnstackedDict = UnstackedSpace.GetPtr<FDictSpace>())
	{
		for (const auto& Pair : UnstackedDict->Spaces)
		{
			for (int32 i = 0; i < N; ++i)
			{
				OutSpace.Spaces.Add(Pair.Key + FString::Printf(TEXT("_%d"), i), Pair.Value);
			}
		}
	}
	else
	{
		for (int32 i = 0; i < N; ++i)
		{
			OutSpace.Spaces.Add(FString::Printf(TEXT("Point_%d"), i), UnstackedSpace);
		}
	}
}
