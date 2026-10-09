// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ActuatorInterface.h"
#include "Points/BoxPoint.h"
#include "Spaces/BoxSpace.h"
#include "Spaces/BoxSpaceDimension.h"
#include "MovementInputActuator.generated.h"

class APawn;

/**
 * @brief Delegate signature for movement input events.
 * 
 * Broadcasts the movement vector when an action is received.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMovementInputSignature, const FVector&, MovementInput);

/**
 * @brief Movement input actuator that applies continuous movement to a Pawn.
 * 
 * Uses AddMovementInput to apply movement along enabled axes (X, Y, Z).
 * Accepts a BoxPoint action where each dimension corresponds to forward/right/up movement speed.
 */
UCLASS(BlueprintType, Blueprintable, ClassGroup = (Schola), meta = (BlueprintSpawnableComponent, DisplayName = "Movement Input Actuator"))
class SCHOLAINTERACTORS_API UMovementInputActuator : public UActorComponent, public IScholaActuator
{
	GENERATED_BODY()

public:
	/** Toggle for whether this actuator moves the Agent along the X dimension (forward/backward) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Actuator Settings")
	bool bHasXDimension = true;

	/** Toggle for whether this actuator moves the Agent along the Y dimension (left/right) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Actuator Settings")
	bool bHasYDimension = true;

	/** Toggle for whether this actuator moves the Agent along the Z dimension (up/down) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Actuator Settings")
	bool bHasZDimension = true;

	/** The minimum speed at which the agent can move. Set to negative to allow for backwards movement along each axis. */
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bHasXDimension || bHasYDimension || bHasZDimension"), Category = "Actuator Settings")
	float MinSpeed = 0.0f;

	/** The maximum speed at which the agent can move. Defines the upper bound of the action space. */
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bHasXDimension || bHasYDimension || bHasZDimension"), Category = "Actuator Settings")
	float MaxSpeed = 1.0f;

	/** Scale to apply to input. See AddMovementInput for more details. */
	UPROPERTY(EditAnywhere, Category = "Actuator Settings")
	float ScaleValue = 1.0f;

	/** Force the Pawn to move. See AddMovementInput for more details. */
	UPROPERTY(EditAnywhere, Category = "Actuator Settings")
	bool bForce = false;

	/** A delegate invoked when this actuator receives input from a brain. Useful for debugging and logging. */
	UPROPERTY(BlueprintAssignable)
	FOnMovementInputSignature OnMovementDelegate;

	/**
	 * @brief Get the action space for this actuator.
	 * 
	 * Returns a BoxSpace with dimensions for each enabled axis (X, Y, Z).
	 * Each dimension is bounded by [MinSpeed, MaxSpeed].
	 * 
	 * @param[out] OutActionSpace The action space definition to be populated
	 */
	virtual void GetActionSpace_Implementation(FInstancedStruct& OutActionSpace) const override;
	
	/**
	 * @brief Apply an action to this actuator.
	 * 
	 * Accepts either a BoxPoint directly or extracts it from a DictPoint.
	 * The action values are interpreted as movement speeds along enabled axes.
	 * 
	 * @param[in] InAction The action to apply (BoxPoint or DictPoint)
	 */
	virtual void TakeAction_Implementation(const FInstancedStruct& InAction) override;

	/**
	 * @brief Typed convenience method for taking action with a BoxPoint directly.
	 * 
	 * @param[in] Action The box point containing movement values for enabled dimensions
	 */
	void TakeAction(const FBoxPoint& Action);

	/**
	 * @brief Generate a unique ID string for this actuator.
	 * 
	 * @return FString describing the actuator configuration (e.g., "MovementInput_X_true_Y_true_Z_false_Min_-1.00_Max_1.00")
	 */
	FString GenerateId() const;

protected:
	/**
	 * @brief Convert a Box Point to an FVector for movement input.
	 * 
	 * Resolves the owning Pawn from GetOwner() and extracts values from the BoxPoint based on
	 * enabled dimensions, mapping them to X, Y, Z components.
	 * 
	 * @param[in] Action BoxPoint that will be converted
	 * @param[out] OutOwnerPawn The component owner cast to APawn, or nullptr if not a Pawn
	 * @return FVector containing the converted movement values
	 */
	FVector ConvertActionToFVector(const FBoxPoint& Action, APawn*& OutOwnerPawn) const;
};
