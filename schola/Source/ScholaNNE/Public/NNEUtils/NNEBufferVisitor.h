// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "NNEUtils/NNEBuffer.h"

/**
 * @brief Visitor interface for processing different types of NNE buffers
 * 
 * This interface implements the visitor pattern, allowing type-safe operations
 * on different buffer types without requiring runtime type checking.
 */
struct SCHOLANNE_API FNNEBufferVisitor
{

public:
	/**
	 * @brief Visit operation for dictionary buffers
	 * @param[in] InBuffer The dictionary buffer to process
	 */
	virtual void operator()(const FNNEDictBuffer& InBuffer) = 0;

	/**
	 * @brief Visit operation for box buffers
	 * @param[in] InBuffer The box buffer to process
	 */
	virtual void operator()(const FNNEBoxBuffer& InBuffer) = 0;

	/**
	 * @brief Visit operation for multi-binary buffers
	 * @param[in] InBuffer The multi-binary buffer to process
	 */
	virtual void operator()(const FNNEMultiBinaryBuffer& InBuffer) = 0;

	/**
	 * @brief Visit operation for discrete buffers
	 * @param[in] InBuffer The discrete buffer to process
	 */
	virtual void operator()(const FNNEDiscreteBuffer& InBuffer) = 0;

	/**
	 * @brief Visit operation for multi-discrete buffers
	 * @param[in] InBuffer The multi-discrete buffer to process
	 */
	virtual void operator()(const FNNEMultiDiscreteBuffer& InBuffer) = 0;

	virtual ~FNNEBufferVisitor() = default;
};