// Copyright (c) 2026 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeExecutionContext.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "Runtime/Launch/Resources/Version.h"

/**
 * @brief Common helper functions for State Tree nodes.
 */
namespace StateTreeHelpers
{
	/**
	 * Gets the context actor from StateTree execution context.
	 * Tries "Actor" first, then falls back to AIController's pawn, then Owner.
	 *
	 * @param Context The State Tree execution context
	 * @return The context actor, or nullptr if not found
	 */
	inline AActor* GetActorFromContext(FStateTreeExecutionContext& Context)
	{
#if ENGINE_MAJOR_VERSION > 5 || (ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 6)
		// UE 5.6+: Use GetContextDataByName API
		FStateTreeDataView ActorDataView = Context.GetContextDataByName(FName("Actor"));
		if (ActorDataView.IsValid())
		{
			if (AActor* Actor = const_cast<AActor*>(ActorDataView.GetPtr<AActor>()))
			{
				return Actor;
			}
		}

		FStateTreeDataView AIControllerView = Context.GetContextDataByName(FName("AIController"));
		if (AIControllerView.IsValid())
		{
			if (const AAIController* AIController = AIControllerView.GetPtr<AAIController>())
			{
				return AIController->GetPawn();
			}
		}
#endif

		// Fallback: Get owner and traverse to actor (works in all UE versions)
		if (UObject* Owner = Context.GetOwner())
		{
			// If owner is an actor, use it directly
			if (AActor* OwnerActor = Cast<AActor>(Owner))
			{
				return OwnerActor;
			}

			// Get outer actor (handles StateTreeComponent and other component cases)
			if (AActor* OuterActor = Owner->GetTypedOuter<AActor>())
			{
				return OuterActor;
			}
		}

		return nullptr;
	}
} // namespace StateTreeHelpers
