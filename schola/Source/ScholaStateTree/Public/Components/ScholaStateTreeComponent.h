// Copyright (c) 2026 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/StateTreeComponent.h"
#include "ScholaStateTreeComponent.generated.h"

/**
 * StateTree component with public accessor for the StateTree asset.
 * Use this component when using AStateTreeTrainingEnvironment so the
 * environment can read the StateTree configuration from the component.
 */
UCLASS(ClassGroup = AI, meta = (BlueprintSpawnableComponent, DisplayName = "Schola StateTree"))
class SCHOLASTATETREE_API UScholaStateTreeComponent : public UStateTreeComponent
{
	GENERATED_BODY()

public:
	/** Get the StateTree asset configured on this component. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Schola|StateTree")
	const UStateTree* GetStateTree() const
	{
		return StateTreeRef.GetStateTree();
	}
};
