// Copyright (c) 2023-2024 Advanced Micro Devices, Inc. All Rights Reserved.

#include "Points/MultiBinaryPoint.h"

void FMultiBinaryPoint::Accept(ConstPointVisitor& Visitor) const
{
	Visitor(*this);
}

void FMultiBinaryPoint::Accept(PointVisitor& Visitor)
{
	Visitor(*this);
}