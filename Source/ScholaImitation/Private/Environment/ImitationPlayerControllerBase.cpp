// Copyright (c) 2026 Advanced Micro Devices, Inc. All Rights Reserved.

#include "Environment/ImitationPlayerControllerBase.h"
#include "ImitationUtils/EnhancedInputUtils.h"
#include "EnhancedPlayerInput.h"
#include "Engine/LocalPlayer.h"

AImitationPlayerControllerBase::AImitationPlayerControllerBase()
{
}

UEnhancedPlayerInput* AImitationPlayerControllerBase::GetEnhancedPlayerInput() const
{
	ULocalPlayer* LP = GetLocalPlayer();
	if (!LP)
	{
		return nullptr;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!Subsystem)
	{
		return nullptr;
	}

	return Subsystem->GetPlayerInput();
}

bool AImitationPlayerControllerBase::BuildActionSpaceFromIMC(UInputMappingContext* MappingContext, FInstancedStruct& OutActionSpace)
{
	return FEnhancedInputUtils::BuildActionSpaceFromIMC(MappingContext, ToTypedInstancedStruct<FSpace>(OutActionSpace));
}

bool AImitationPlayerControllerBase::PollActionsFromIMC(UInputMappingContext* MappingContext, FInstancedStruct& OutActions)
{
	return FEnhancedInputUtils::PollActionsFromIMC(MappingContext, GetEnhancedPlayerInput(), ToTypedInstancedStruct<FPoint>(OutActions));
}
