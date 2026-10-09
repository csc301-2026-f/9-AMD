// Copyright (c) 2026 Advanced Micro Devices, Inc. All Rights Reserved.

#include "ImitationUtils/EnhancedInputUtils.h"
#include "EnhancedPlayerInput.h"
#include "Spaces/DictSpace.h"
#include "Spaces/BoxSpace.h"
#include "Spaces/MultiBinarySpace.h"
#include "Points/DictPoint.h"
#include "LogScholaImitation.h"
#include "Points/BoxPoint.h"
#include "Points/MultiBinaryPoint.h"

bool FEnhancedInputUtils::BuildActionSpaceFromIMC(UInputMappingContext* MappingContext, TInstancedStruct<FSpace>& OutActionSpace)
{
	FDictSpace DictSpace;

	if (!MappingContext)
	{
		UE_LOGFMT(LogScholaImitation, Warning, "FEnhancedInputUtils::BuildActionSpaceFromIMC(): MappingContext is null.");
		return false;
	}

	// Get unique actions from the mapping context
	TSet<const UInputAction*>				 UniqueActions;
	const TArray<FEnhancedActionKeyMapping>& Mappings = MappingContext->GetMappings();

	for (const FEnhancedActionKeyMapping& Mapping : Mappings)
	{
		if (Mapping.Action && !UniqueActions.Contains(Mapping.Action.Get()))
		{
			UniqueActions.Add(Mapping.Action.Get());

			const UInputAction*		 Action = Mapping.Action.Get();
			TInstancedStruct<FSpace> SpaceStruct;

			// Create appropriate space type based on action value type
			switch (Action->ValueType)
			{
				case EInputActionValueType::Boolean:
				{
					SpaceStruct.InitializeAs<FMultiBinarySpace>(1);
					break;
				}
				case EInputActionValueType::Axis1D:
				{
					SpaceStruct.InitializeAs<FBoxSpace>(TArray<float>{ -1.0f }, TArray<float>{ 1.0f });
					break;
				}
				case EInputActionValueType::Axis2D:
				{
					SpaceStruct.InitializeAs<FBoxSpace>(TArray<float>{ -1.0f, -1.0f }, TArray<float>{ 1.0f, 1.0f });
					break;
				}
				case EInputActionValueType::Axis3D:
				{
					SpaceStruct.InitializeAs<FBoxSpace>(TArray<float>{ -1.0f, -1.0f, -1.0f }, TArray<float>{ 1.0f, 1.0f, 1.0f });
					break;
				}
				default:
				{
					UE_LOGFMT(LogScholaImitation, Warning, "FEnhancedInputUtils::BuildActionSpaceFromIMC(): Unsupported EInputActionValueType ({0}) for action '{1}'.",
						static_cast<int32>(Action->ValueType),
						Action->GetName());
					break;
				}
			}

			DictSpace.Spaces.Add(Action->GetFName().ToString(), MoveTemp(SpaceStruct));
		}
	}

	OutActionSpace.InitializeAs<FDictSpace>(DictSpace);
	return true;
}

bool FEnhancedInputUtils::PollActionsFromIMC(UInputMappingContext* MappingContext, UEnhancedPlayerInput* EnhancedInput, TInstancedStruct<FPoint>& OutActions)
{
	FDictPoint DictPoint;

	if (!MappingContext)
	{
		UE_LOGFMT(LogScholaImitation, Error, "FEnhancedInputUtils::PollActionsFromIMC(): MappingContext is null.");
		return false;
	}

	if (!EnhancedInput)
	{
		UE_LOGFMT(LogScholaImitation, Error, "FEnhancedInputUtils::PollActionsFromIMC(): EnhancedInput is null.");
		return false;
	}

	// Get unique actions from the mapping context
	TSet<const UInputAction*>				 UniqueActions;
	const TArray<FEnhancedActionKeyMapping>& Mappings = MappingContext->GetMappings();

	for (const FEnhancedActionKeyMapping& Mapping : Mappings)
	{
		if (Mapping.Action && !UniqueActions.Contains(Mapping.Action.Get()))
		{
			UniqueActions.Add(Mapping.Action.Get());

			const UInputAction*		 Action = Mapping.Action.Get();
			FInputActionValue		 ActionValue = EnhancedInput->GetActionValue(Action);
			TInstancedStruct<FPoint> PointStruct;

			// Convert to appropriate point type based on action value type
			switch (Action->ValueType)
			{
				case EInputActionValueType::Boolean:
				{
					bool bValue = ActionValue.Get<bool>();
					PointStruct.InitializeAs<FMultiBinaryPoint>(TArray<bool>{ bValue });
					break;
				}
				case EInputActionValueType::Axis1D:
				{
					float Value = ActionValue.Get<float>();
					PointStruct.InitializeAs<FBoxPoint>(TArray<float>{ Value });
					break;
				}
				case EInputActionValueType::Axis2D:
				{
					FVector2D Value = ActionValue.Get<FVector2D>();
					PointStruct.InitializeAs<FBoxPoint>(TArray<float>{ (float)Value.X, (float)Value.Y });
					break;
				}
				case EInputActionValueType::Axis3D:
				{
					FVector Value = ActionValue.Get<FVector>();
					PointStruct.InitializeAs<FBoxPoint>(TArray<float>{ (float)Value.X, (float)Value.Y, (float)Value.Z });
					break;
				}
				default:
				{
					UE_LOGFMT(LogScholaImitation, Warning, "FEnhancedInputUtils::PollActionsFromIMC(): Unsupported EInputActionValueType ({0}) for action '{1}'. Skipping.",
						static_cast<int32>(Action->ValueType),
						Action->GetFName().ToString());
					continue;
				}
			}

			DictPoint.Points.Add(Action->GetFName().ToString(), MoveTemp(PointStruct));
		}
	}

	OutActions.InitializeAs<FDictPoint>(DictPoint);
	return true;
}
