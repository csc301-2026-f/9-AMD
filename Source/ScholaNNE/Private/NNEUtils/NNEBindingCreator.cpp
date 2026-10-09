// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "NNEUtils/NNEBindingCreator.h"
#include "LogScholaNNE.h"


//TODO this needs some error handling for when things don't line up properly, e.g. if the space is not found in the buffer map.
//Note this can happen for Stateful models, since those will have IO for state that isn't in the space. passed for binding
// ~acann, 7/4/2025

void FNNEBindingCreator::operator()(const FNNEDictBuffer& InBuffer)
{
	// New export format: inputs/outputs are no longer flattened
	// Each dict key maps to a separate named tensor in the ONNX model
	// We need to iterate through the tensor descriptors and match them to dict buffer keys
	
	for (; this->Index < TensorDescs.Num(); this->Index++)
	{
		const FString& TensorName = TensorDescs[this->Index].GetName();
		
		// Skip state tensors as they're handled separately
		if(TensorName.StartsWith(TEXT("state_in")) || TensorName.StartsWith(TEXT("state_out")))
		{
			continue;
		}
		
		if (this->Index >= Bindings.Num())
		{
			UE_LOGFMT(LogScholaNNE, Error, "FNNEBindingCreator::operator(): Index {0} out of bounds for Bindings array of size {1}", this->Index, Bindings.Num());
			this->bError = true;
			return;
		}
		
		// Try to find a buffer with the same name as the tensor
		const TInstancedStruct<FNNEPointBuffer>* PointBuffer = InBuffer.Buffers.Find(TensorName);
		
		if (PointBuffer && PointBuffer->IsValid())
		{
			// Process the sub-buffer to create its binding
			PointBuffer->Get<FNNEPointBuffer>().Accept(*this);
		}
		else
		{
			UE_LOGFMT(LogScholaNNE, Error, "FNNEBindingCreator::operator(): Key {0} not found or invalid in Dict Buffer", TensorName);
			this->bError = true;
			return;
		}
	}
}

void FNNEBindingCreator::operator()(const FNNEBoxBuffer& InBuffer)
{
	Bindings[this->Index] = InBuffer.MakeBinding();
}

void FNNEBindingCreator::operator()(const FNNEMultiBinaryBuffer& InBuffer)
{
	Bindings[this->Index] = InBuffer.MakeBinding();
}

void FNNEBindingCreator::operator()(const FNNEDiscreteBuffer& InBuffer)
{
	Bindings[this->Index] = InBuffer.MakeBinding();
}

void FNNEBindingCreator::operator()(const FNNEMultiDiscreteBuffer& InBuffer)
{
	Bindings[this->Index] = InBuffer.MakeBinding();
}
