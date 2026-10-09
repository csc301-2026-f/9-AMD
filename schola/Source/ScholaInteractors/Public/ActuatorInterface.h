// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
// #include "StructUtils/InstancedStruct.h"
#include "Points/Point.h"
#include "Spaces/Space.h"
#include "Common/InstancedStructUtils.h"
extern template struct TInstancedStruct<FPoint>;
#include "ActuatorInterface.generated.h"


/**
 * @brief Actuator interface for Schola reinforcement learning agents.
 * 
 * Implementers return a description of their action Space and accept actions (Points).
 * This interface allows agents to interact with the environment by taking actions.
 */
UINTERFACE(BlueprintType)
class SCHOLAINTERACTORS_API UScholaActuator : public UInterface
{
	GENERATED_BODY()
};

/**
 * @brief Interface for actuator components that receive and execute agent actions.
 * 
 * This interface provides methods for defining the action space and processing actions
 * from reinforcement learning agents. Can be implemented in both C++ and Blueprints.
 */
class SCHOLAINTERACTORS_API IScholaActuator
{
	GENERATED_BODY()

public:
	
    /**
     * @brief Initialize the actuator component.
     * 
     * Called once to set up the actuator. Override this method to perform any
     * necessary initialization such as caching references or validating configuration.
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Actuator")
    void InitActuator();

    /**
     * @brief Default implementation of InitActuator (no-op).
     */
    virtual void InitActuator_Implementation(){};

    /**
	 * @brief Return the actuator definition (the action space) as an instanced FSpace struct.
	 * 
	 * This method defines what actions are valid for this actuator. The returned space
	 * describes the structure, dimensionality, and bounds of the action space.
	 * BlueprintNativeEvent so Blueprints can override; C++ classes can implement the _Implementation.
	 * 
	 * @param[out] OutActionSpace The action space definition to be populated
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Actuator")
	void GetActionSpace(FInstancedStruct& OutActionSpace) const;

	/**
	 * @brief Static helper to execute GetActionSpace with typed parameters.
	 * 
	 * @param[in] Obj The object implementing this interface
	 * @param[out] OutActionSpace The action space to be populated
	 */
	static void Execute_GetActionSpace(const UObject* Obj, TInstancedStruct<FSpace>& OutActionSpace)
	{
		IScholaActuator::Execute_GetActionSpace(Obj, ToUntypedInstancedStruct(OutActionSpace));
	};

	/**
	 * @brief Default C++ implementation returns a default box space.
	 * 
	 * @param[out] OutActionSpace The action space definition to be populated
	 */
	virtual void GetActionSpace_Implementation(FInstancedStruct& OutActionSpace) const {};

	/**
	 * @brief Apply an action to the actuator provided as an instanced FPoint struct.
	 * 
	 * This method receives an action from a brain/policy and applies it to the environment.
	 * The action format must match the action space returned by GetActionSpace.
	 * BlueprintNativeEvent so Blueprints can override; C++ classes can implement the _Implementation.
	 * 
	 * @param[in] InAction The action to apply, must be compatible with this actuator's action space
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Actuator")
	void TakeAction(const FInstancedStruct& InAction);

	/**
	 * @brief Static helper to execute TakeAction with typed parameters.
	 * 
	 * @param[in,out] Obj The object implementing this interface
	 * @param[in] InAction The action to apply
	 */
	static void Execute_TakeAction(UObject* Obj, const TInstancedStruct<FPoint>& InAction)
	{
		IScholaActuator::Execute_TakeAction(Obj, ToUntypedInstancedStruct(InAction));
	};

	/**
	 * @brief Default C++ implementation is a no-op.
	 * 
	 * @param[in] InAction The action to apply (unused in default implementation)
	 */
	virtual void TakeAction_Implementation(const FInstancedStruct& InAction){};
};
