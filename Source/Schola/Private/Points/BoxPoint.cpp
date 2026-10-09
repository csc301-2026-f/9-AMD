// Copyright (c) 2023-2024 Advanced Micro Devices, Inc. All Rights Reserved.

#include "Points/BoxPoint.h"

void FBoxPoint::Accept(PointVisitor& Visitor)
{
	Visitor(*this);
}

void FBoxPoint::Accept(ConstPointVisitor& Visitor) const
{
	Visitor(*this);
}