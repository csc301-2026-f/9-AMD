// Copyright (c) 2023 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NNE.h"
#include "NNERuntimeCPU.h"
#include "NNERuntimeGPU.h"
#include "NNEModelData.h"
#include "NNERuntimeRunSync.h"
#include <type_traits>
#include "NNEStatus.h"
#include "NNEWrappers.generated.h"

using UE::NNE::FTensorBindingCPU;
using UE::NNE::IModelInstanceRunSync;

/**
 * @brief Generic interface for neural network model instances across different device types
 * 
 * This interface provides a device-agnostic way to interact with NNE model instances,
 * abstracting differences between CPU and GPU implementations.
 */
class IModelInstanceInterface
{
public:
	virtual ~IModelInstanceInterface() = default;
	
	/**
	 * @brief Gets the input tensor descriptors for the model
	 * @return Array view of input tensor descriptors
	 */
	virtual TConstArrayView<UE::NNE::FTensorDesc> GetInputTensorDescs() = 0;
	
	/**
	 * @brief Sets the input tensor shapes for the model
	 * @param[in] InInputShapes Array of tensor shapes to set
	 * @return Status indicating success or failure
	 */
	virtual UE::NNE::EResultStatus SetInputTensorShapes(TConstArrayView<UE::NNE::FTensorShape> InInputShapes) = 0;
	
	/**
	 * @brief Runs synchronous inference on the model
	 * @param[in] InInputBindings Input tensor bindings containing the input data
	 * @param[in,out] InOutputBinding Output tensor bindings to receive the results
	 * @return Status indicating success or failure
	 */
	virtual UE::NNE::EResultStatus RunSync(TConstArrayView<FTensorBindingCPU> InInputBindings, TConstArrayView<FTensorBindingCPU> InOutputBinding) = 0;
};

/**
 * @brief Generic interface for neural network models across different device types
 * 
 * This interface provides a device-agnostic way to create and validate neural network models,
 * abstracting differences between CPU and GPU implementations.
 */
class IModelInterface
{

public:
	virtual ~IModelInterface() = default;

	/**
	 * @brief Creates a model instance for running inference
	 * @return Shared pointer to the created model instance
	 */
	virtual TSharedPtr<IModelInstanceRunSync> CreateModelInstance() = 0;
	
	/**
	 * @brief Checks if the model is valid and ready for use
	 * @return true if the model is valid, false otherwise
	 */
	virtual bool IsValid() = 0;
};

/**
 * @brief Wrapper class for CPU-based neural network models
 * 
 * This class wraps NNE CPU models to provide a consistent interface
 * for creating model instances and checking validity.
 */
class FCPUModelWrapper : public IModelInterface
{
	/** Pointer to the underlying NNE CPU model */
	TSharedPtr<UE::NNE::IModelCPU> ModelPtr;

public:
	/**
	 * @brief Constructor that wraps a raw CPU model
	 * @param[in] RawModel The NNE CPU model to wrap
	 */
	FCPUModelWrapper(TSharedPtr<UE::NNE::IModelCPU> RawModel)
		: ModelPtr(RawModel) {

		};

	/**
	 * @brief Checks if the wrapped model is valid
	 * @return true if the model pointer is valid, false otherwise
	 */
	bool IsValid()
	{
		return ModelPtr.IsValid();
	};

	/**
	 * @brief Creates a CPU model instance for inference
	 * @return Shared pointer to the created CPU model instance
	 */
	TSharedPtr<IModelInstanceRunSync> CreateModelInstance()
	{
		return this->ModelPtr->CreateModelInstanceCPU();
	};
};

/**
 * @brief Wrapper class for GPU-based neural network models
 * 
 * This class wraps NNE GPU models to provide a consistent interface
 * for creating model instances and checking validity.
 */
class FGPUModelWrapper : public IModelInterface
{
	/** Pointer to the underlying NNE GPU model */
	TSharedPtr<UE::NNE::IModelGPU> ModelPtr;

public:
	/**
	 * @brief Constructor that wraps a raw GPU model
	 * @param[in] RawModel The NNE GPU model to wrap
	 */
	FGPUModelWrapper(TSharedPtr<UE::NNE::IModelGPU> RawModel)
		: ModelPtr(RawModel) {

		};

	/**
	 * @brief Checks if the wrapped model is valid
	 * @return true if the model pointer is valid, false otherwise
	 */
	bool IsValid()
	{
		return ModelPtr.IsValid();
	};

	/**
	 * @brief Creates a GPU model instance for inference
	 * @return Shared pointer to the created GPU model instance
	 */
	TSharedPtr<IModelInstanceRunSync> CreateModelInstance()
	{
		return this->ModelPtr->CreateModelInstanceGPU();
	};
};

/**
 * @brief Generic interface for neural network runtimes across different device types
 * 
 * This interface provides a device-agnostic way to create models and check runtime validity,
 * abstracting differences between CPU and GPU runtimes.
 */
class IRuntimeInterface
{
public:
	virtual ~IRuntimeInterface() = default;
	
	/**
	 * @brief Creates a model from model data
	 * @param[in] ModelData The neural network model data to load
	 * @return Unique pointer to the created model interface
	 */
	virtual TUniquePtr<IModelInterface> CreateModel(TObjectPtr<UNNEModelData> ModelData) = 0;
	
	/**
	 * @brief Checks if the runtime is valid and ready for use
	 * @return true if the runtime is valid, false otherwise
	 */
	virtual bool IsValid() = 0;
};

/**
 * @brief Wrapper class for CPU-based neural network runtimes
 * 
 * This class wraps NNE CPU runtimes to provide a consistent interface
 * for creating models and checking validity.
 */
class FCPURuntimeWrapper : public IRuntimeInterface
{
	/** Weak pointer to the underlying NNE CPU runtime */
	TWeakInterfacePtr<INNERuntimeCPU> RuntimePtr;

public:
	/**
	 * @brief Constructor that wraps a raw CPU runtime
	 * @param[in] RawRuntime The NNE CPU runtime to wrap
	 */
	FCPURuntimeWrapper(TWeakInterfacePtr<INNERuntimeCPU> RawRuntime)
		: RuntimePtr(RawRuntime)
	{
	}

	/**
	 * @brief Checks if the wrapped runtime is valid
	 * @return true if the runtime pointer is valid, false otherwise
	 */
	bool IsValid()
	{
		return RuntimePtr.IsValid();
	};

	/**
	 * @brief Creates a CPU model from model data
	 * @param[in] ModelData The neural network model data to load
	 * @return Unique pointer to the created CPU model wrapper
	 */
	TUniquePtr<IModelInterface> CreateModel(TObjectPtr<UNNEModelData> ModelData)
	{
		return TUniquePtr<IModelInterface>(new FCPUModelWrapper(this->RuntimePtr->CreateModelCPU(ModelData)));
	};
};

/**
 * @brief Wrapper class for GPU-based neural network runtimes
 * 
 * This class wraps NNE GPU runtimes to provide a consistent interface
 * for creating models and checking validity.
 */
class FGPURuntimeWrapper : public IRuntimeInterface
{
	/** Weak pointer to the underlying NNE GPU runtime */
	TWeakInterfacePtr<INNERuntimeGPU> RuntimePtr;

public:
	/**
	 * @brief Constructor that wraps a raw GPU runtime
	 * @param[in] RawRuntime The NNE GPU runtime to wrap
	 */
	FGPURuntimeWrapper(TWeakInterfacePtr<INNERuntimeGPU> RawRuntime)
		: RuntimePtr(RawRuntime)
	{
	}

	/**
	 * @brief Checks if the wrapped runtime is valid
	 * @return true if the runtime pointer is valid, false otherwise
	 */
	bool IsValid()
	{
		return RuntimePtr.IsValid();
	};

	/**
	 * @brief Creates a GPU model from model data
	 * @param[in] ModelData The neural network model data to load
	 * @return Unique pointer to the created GPU model wrapper
	 */
	TUniquePtr<IModelInterface> CreateModel(TObjectPtr<UNNEModelData> ModelData)
	{
		return TUniquePtr<IModelInterface>(new FGPUModelWrapper(this->RuntimePtr->CreateModelGPU(ModelData)));
	};
};

/**
 * @brief Enumeration for different runtime device types
 * 
 * This enum distinguishes between CPU and GPU runtime execution targets.
 */
UENUM(BlueprintType)
enum class ERuntimeType : uint8
{
	CPU, /** CPU-based runtime execution */
	GPU  /** GPU-based runtime execution */
};