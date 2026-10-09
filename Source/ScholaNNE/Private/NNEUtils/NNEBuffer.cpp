// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.
#include "NNEUtils/NNEBuffer.h"
#include "NNEUtils/NNEBufferVisitor.h"

void FNNEDictBuffer::Accept(FNNEBufferVisitor& Visitor) const
{
	Visitor(*this);
}

void FNNEDiscreteBuffer::Accept(FNNEBufferVisitor& Visitor) const
{
	Visitor(*this);
}

void FNNEMultiBinaryBuffer::Accept(FNNEBufferVisitor& Visitor) const
{
	Visitor(*this);
}

void FNNEBoxBuffer::Accept(FNNEBufferVisitor& Visitor) const
{
	Visitor(*this);
}

void FNNEMultiDiscreteBuffer::Accept(FNNEBufferVisitor& Visitor) const
{
	Visitor(*this);
}
