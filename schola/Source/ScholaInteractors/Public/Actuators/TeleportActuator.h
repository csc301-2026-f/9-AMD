// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ActuatorInterface.h"
#include "Spaces/MultiDiscreteSpace.h"
#include "Points/MultiDiscretePoint.h"
#include "TeleportActuator.generated.h"

/**
 * @brief Bitflags for enabling teleport movement directions per dimension.
 * 
 * Each dimension (X, Y, Z) can independently enable Positive and/or Negative movement.
 */
UENUM(BlueprintType, Meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class ETeleportDimensionFlags : uint8
{
	Default = 0 UMETA(Hidden),
	None = 1 << 0, /** Enable not taking movement/waiting in this direction (0x01) */
    Forwards = 1 << 1,   /** Enable positive/forward movement (0x02) */
    Backwards = 1 << 2,   /** Enable negative/backward movement (0x04) */
};

ENUM_CLASS_FLAGS(ETeleportDimensionFlags);


/**
 * @brief Delegate signature for teleport events.
 * 
 * Broadcasts the teleport offset vector when an action is received.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTeleportComponentSignature, const FVector&, TeleportOffset);

/**
 * @brief Teleport actuator that applies discrete position changes to an Actor.
 * 
 * Implements IScholaActuator directly as a component without additional base classes.
 * Exposes a MultiDiscrete action space based on enabled translation directions (X,Y,Z).
 * Each dimension uses bitflags to independently enable Positive and/or Negative movement.
 * Actions move the actor by the configured step distance in the allowed directions.
 */
UCLASS(BlueprintType, Blueprintable, ClassGroup = (Schola), meta = (BlueprintSpawnableComponent, DisplayName = "Teleport Actuator"))
class SCHOLAINTERACTORS_API UTeleportActuator : public UActorComponent, public IScholaActuator
{
    GENERATED_BODY()
public:
    /** Bitflags to enable positive/negative X translation */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Bitmask, BitmaskEnum="/Script/ScholaInteractors.ETeleportDimensionFlags"), Category="Actuator Settings")
    uint8 XMovementDirectionFlags = static_cast<uint8>(ETeleportDimensionFlags::Forwards) | static_cast<uint8>(ETeleportDimensionFlags::Backwards);

    /** Step distance along local X when action digit selects Forward/Backward */
    UPROPERTY(EditAnywhere, meta=(EditCondition="(XMovementDirectionFlags & (XMovementDirectionFlags - 1)) != 0"), Category="Actuator Settings")
    float XStep = 100.f;

    /** Bitflags to enable positive/negative Y translation */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Bitmask, BitmaskEnum="/Script/ScholaInteractors.ETeleportDimensionFlags"), Category="Actuator Settings")
    uint8 YMovementDirectionFlags = static_cast<uint8>(ETeleportDimensionFlags::Forwards) | static_cast<uint8>(ETeleportDimensionFlags::Backwards);

    /** Step distance along local Y when action digit selects Forward/Backward */
    UPROPERTY(EditAnywhere, meta=(EditCondition="(YMovementDirectionFlags & (YMovementDirectionFlags - 1)) != 0"), Category="Actuator Settings")
    float YStep = 0.f;

    /** Bitflags to enable positive/negative Z translation */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Bitmask, BitmaskEnum="/Script/ScholaInteractors.ETeleportDimensionFlags"), Category="Actuator Settings")
    uint8 ZMovementDirectionFlags = 0;

    /** Step distance along local Z when action digit selects Forward/Backward */
    UPROPERTY(EditAnywhere, meta=(EditCondition="(ZMovementDirectionFlags & (ZMovementDirectionFlags - 1)) != 0"), Category="Actuator Settings")
    float ZStep = 0.f;

    /** Type of teleportation (passed to SetActorLocation). Determines physics behavior during movement. */
    UPROPERTY(EditAnywhere, Category="Actuator Settings")
    ETeleportType TeleportType = ETeleportType::None;

    /** Whether to sweep when moving. If true, the actor will check for collisions during movement. */
    UPROPERTY(EditAnywhere, Category="Actuator Settings")
    bool bSweep = false;

    /** Broadcast after applying a teleport delta. Useful for debugging and logging. */
    UPROPERTY(BlueprintAssignable)
    FOnTeleportComponentSignature OnTeleportDelegate;

    /**
     * @brief Get the action space for this actuator.
     * 
     * Returns a MultiDiscreteSpace with dimensions based on enabled direction flags.
     * Each dimension represents the available movements (None/Forward/Backward) for axes
     * that have at least one direction enabled via bitflags.
     * 
     * @param[out] OutActionSpace The action space definition to be populated
     */
    virtual void GetActionSpace_Implementation(FInstancedStruct& OutActionSpace) const override;
    
    /**
     * @brief Apply a teleport action to the owner actor.
     * 
     * Converts the MultiDiscretePoint action to a position delta and applies it.
     * 
     * @param[in] InAction The action to apply (MultiDiscretePoint)
     */
	virtual void TakeAction_Implementation(const FInstancedStruct& InAction) override;

    /**
     * @brief Generate a unique ID string for this actuator.
     * 
     * @return FString describing the actuator configuration (e.g., "Teleport_X_100.00_Y_0.00_Z_0.00")
     */
    FString GenerateId() const;

    /**
     * @brief Typed convenience method for taking action with a MultiDiscretePoint directly.
     * 
     * @param[in] ActionPoint The multi-discrete action containing movement directions for each enabled axis
     */
    void TakeAction(const FMultiDiscretePoint& ActionPoint);

protected:
    /**
     * @brief Convert a MultiDiscretePoint action to a world-space movement vector.
     * 
     * Interprets each discrete action value as None/Forward/Backward and scales by the step distances.
     * 
     * @param[in] ActionPoint The action to convert
     * @return FVector containing the movement delta
     */
    FVector ConvertActionToVector(const FMultiDiscretePoint& ActionPoint) const;
    
    /**
     * @brief Calculate the movement distance for a single axis based on discrete action.
     * 
     * @param[in] Step The step distance configured for this axis
     * @param[in] DimensionFlags The bitflags for this dimension indicating which directions are enabled
     * @param[in] Digit The discrete action index (0-based) into the set bits of DimensionFlags
     * @return float The signed movement distance
     */
    float DimensionValue(float Step, uint8 DimensionFlags, int Digit) const;
};
