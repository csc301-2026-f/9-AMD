// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "StructUtils/InstancedStruct.h"
#include "Points/Point.h"
#include "Spaces/Space.h"
#include "Points/BoxPoint.h"
#include "Spaces/BoxSpace.h"
#include "Common/InstancedStructUtils.h"
#include "SensorInterface.generated.h"

/**
 * @brief Sensor interface for Schola reinforcement learning agents.
 * 
 * Implementers return a description of their observation Space and produce observations (Points).
 * This interface allows agents to perceive the environment state.
 */
UINTERFACE(BlueprintType)
class SCHOLAINTERACTORS_API UScholaSensor : public UInterface
{
	GENERATED_BODY()
};

/**
 * @brief Interface for sensor components that collect observations from the environment.
 * 
 * This interface provides methods for defining the observation space and collecting
 * observations for reinforcement learning agents. Can be implemented in both C++ and Blueprints.
 */
class SCHOLAINTERACTORS_API IScholaSensor
{
	GENERATED_BODY()

public:
	
    /**
     * @brief Initialize the sensor component.
     * 
     * Called once to set up the sensor. Override this method to perform any
     * necessary initialization such as creating render targets or validating configuration.
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Sensor")
	void InitSensor();

    /**
     * @brief Default implementation of InitSensor (no-op).
     */
    virtual void InitSensor_Implementation()
    {
        // no-op
    }

    /**
	 * @brief Return the sensor definition (the observation space) as an instanced FSpace struct.
	 * 
	 * This method defines the structure of observations this sensor produces. The returned space
	 * describes the dimensionality, data type, and bounds of the observation space.
	 * BlueprintNativeEvent so Blueprints can override; C++ classes can implement the _Implementation.
	 * 
	 * @param[out] OutObservationSpace The observation space definition to be populated
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Sensor")
	void GetObservationSpace(FInstancedStruct& OutObservationSpace) const;

	/**
	 * @brief Default C++ implementation returns an empty box space.
	 * 
	 * @param[out] OutObservationSpace The observation space to be populated with default box space
	 */
	virtual void GetObservationSpace_Implementation(FInstancedStruct& OutObservationSpace) const
	{
		OutObservationSpace.InitializeAs<FBoxSpace>();
	}

	/**
	 * @brief Static helper to execute GetObservationSpace with typed parameters.
	 * 
	 * @param[in] Obj The object implementing this interface
	 * @param[out] OutObservationSpace The observation space to be populated
	 */
	static void Execute_GetObservationSpace(const UObject* Obj, TInstancedStruct<FSpace>& OutObservationSpace)
	{
		IScholaSensor::Execute_GetObservationSpace(Obj, ToUntypedInstancedStruct(OutObservationSpace));
	};

	/**
	 * @brief Collect a single observation from the sensor as an instanced FPoint struct.
	 * 
	 * This method captures the current state of the environment as perceived by this sensor.
	 * The observation format must match the observation space returned by GetObservationSpace.
	 * BlueprintNativeEvent so Blueprints can override; C++ classes can implement the _Implementation.
	 * 
	 * @param[out] OutObservations The collected observation data to be populated
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Sensor")
	void CollectObservations(FInstancedStruct& OutObservations);

	/**
	 * @brief Static helper to execute CollectObservations with typed parameters.
	 * 
	 * @param[in,out] Obj The object implementing this interface
	 * @param[out] OutObservations The observation data to be populated
	 */
	static void Execute_CollectObservations(UObject* Obj, TInstancedStruct<FPoint>& OutObservations)
	{
		IScholaSensor::Execute_CollectObservations(Obj, ToUntypedInstancedStruct(OutObservations));
	};

	/**
	 * @brief Default C++ implementation returns an empty box point.
	 * 
	 * @param[out] OutObservations The observation data to be populated with default box point
	 */
	virtual void CollectObservations_Implementation(FInstancedStruct& OutObservations)
	{
		OutObservations.InitializeAs<FBoxPoint>();
	}
};
