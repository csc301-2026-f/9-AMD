// Copyright (c) 2023-2024 Advanced Micro Devices, Inc. All Rights Reserved.

#include "Points/MultiDiscretePoint.h"

void FMultiDiscretePoint::Accept(ConstPointVisitor& Visitor) const
{
	Visitor(*this);
}

void FMultiDiscretePoint::Accept(PointVisitor& Visitor)
{
	Visitor(*this);
}