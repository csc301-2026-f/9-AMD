// Copyright (c) 2023 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Common/InteractionDefinition.h"
#include "Common/InstancedStructUtils.h"
#include "Spaces/Space.h"
#include "Policies/PolicyInterface.h"
#include "Points/Point.h"
#include "BlueprintPolicy.generated.h"

/**
 * @class UBlueprintPolicy
 * @brief Abstract base class for Blueprint-implementable policies.
 * 
 * This class allows policies to be implemented entirely in Blueprint, providing
 * a convenient way to create custom decision-making logic without C++ code.
 * Derive from this class in Blueprint and implement the Think and Init events
 * to create a custom policy.
 */
UCLASS(Blueprintable, BlueprintType, Abstract, EditInlineNew)
class SCHOLA_API UBlueprintPolicy : public UObject, public IPolicy
{
	GENERATED_BODY()

public:

	/**
	 * @brief Native implementation of Think that forwards to the Blueprint event.
	 * @param[in] InObservations The observations from the environment.
	 * @param[out] OutAction Output parameter that receives the generated action.
	 * @return True if inference succeeded.
	 */
	bool Think(const TInstancedStruct<FPoint>& InObservations, TInstancedStruct<FPoint>& OutAction) override
	{
		this->Think(ToUntypedInstancedStruct(InObservations), ToUntypedInstancedStruct(OutAction));
		return true;
	}

	/**
	 * @brief Blueprint event for generating actions from observations.
	 * 
	 * Implement this event in Blueprint to define how observations are
	 * converted into actions.
	 * 
	 * @param[in] InObservations The observations from the environment.
	 * @param[out] OutAction Output parameter that receives the generated action.
	 */
	UFUNCTION(BlueprintCallable,BlueprintImplementableEvent, Category = "Schola|Policy")
	void Think(const FInstancedStruct& InObservations, FInstancedStruct& OutAction);



	/**
	 * @brief Blueprint event for initializing the policy.
	 * 
	 * Implement this event in Blueprint to set up the policy when it is
	 * first created or when the interaction definition changes.
	 * 
	 * @param[in] InPolicyDefinition The interaction definition specifying observation and action spaces.
	 * @return True if initialization succeeded, false otherwise.
	 */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Schola|Policy")
	bool Init(const FInteractionDefinition& InPolicyDefinition) override;

};

