// Copyright (c) 2023-2024 Advanced Micro Devices, Inc. All Rights Reserved.

#include "Points/DiscretePoint.h"

void FDiscretePoint::Accept(ConstPointVisitor& Visitor) const
{
	Visitor(*this);
}

void FDiscretePoint::Accept(PointVisitor& Visitor)
{
	Visitor(*this);
}