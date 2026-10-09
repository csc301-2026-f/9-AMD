// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "Actuators/MovementInputActuator.h"
#include "LogScholaInteractors.h"
#include "GameFramework/Pawn.h"
#include "Points/DictPoint.h"

void UMovementInputActuator::GetActionSpace_Implementation(FInstancedStruct& OutActionSpace) const
{
	// Build a BoxSpace with dimensions for each enabled axis
	TArray<FBoxSpaceDimension> Dimensions;

	if (bHasXDimension)
	{
		Dimensions.Add(FBoxSpaceDimension(MinSpeed, MaxSpeed));
	}

	if (bHasYDimension)
	{
		Dimensions.Add(FBoxSpaceDimension(MinSpeed, MaxSpeed));
	}

	if (bHasZDimension)
	{
		Dimensions.Add(FBoxSpaceDimension(MinSpeed, MaxSpeed));
	}

	FBoxSpace ActionSpace(Dimensions);
	OutActionSpace.InitializeAs<FBoxSpace>(ActionSpace);
}

void UMovementInputActuator::TakeAction_Implementation(const FInstancedStruct& InAction)
{
	// Try to get BoxPoint directly
	if (const FBoxPoint* BoxAction = InAction.GetPtr<FBoxPoint>())
	{
		TakeAction(*BoxAction);
		return;
	}

	UE_LOGFMT(LogScholaInteractors, Warning, "UMovementInputActuator::TakeAction_Implementation(): Received action is {0} not a BoxPoint or DictPoint - {1}", InAction.GetScriptStruct() ? InAction.GetScriptStruct()->GetName() : TEXT("null"), GetName());
}

void UMovementInputActuator::TakeAction(const FBoxPoint& Action)
{
	APawn* OwnerPawn = nullptr;
	FVector MovementInput = ConvertActionToFVector(Action, OwnerPawn);

	// Broadcast the delegate for debugging/logging
	OnMovementDelegate.Broadcast(MovementInput);

	// Apply movement if owner resolved to a Pawn
	if (OwnerPawn)
	{
		// Apply movement along each enabled axis
		if (bHasXDimension && MovementInput.X != 0.0f)
		{
			OwnerPawn->AddMovementInput(OwnerPawn->GetActorForwardVector(), MovementInput.X * ScaleValue, bForce);
		}

		if (bHasYDimension && MovementInput.Y != 0.0f)
		{
			OwnerPawn->AddMovementInput(OwnerPawn->GetActorRightVector(), MovementInput.Y * ScaleValue, bForce);
		}

		if (bHasZDimension && MovementInput.Z != 0.0f)
		{
			OwnerPawn->AddMovementInput(OwnerPawn->GetActorUpVector(), MovementInput.Z * ScaleValue, bForce);
		}
	}
	else
	{
		UE_LOGFMT(LogScholaInteractors, Warning, "UMovementInputActuator::TakeAction(): Owner is not a Pawn - cannot apply movement input - {0}", GetName());
	}
}

FVector UMovementInputActuator::ConvertActionToFVector(const FBoxPoint& Action, APawn*& OutOwnerPawn) const
{
	OutOwnerPawn = Cast<APawn>(GetOwner());

	FVector Result = FVector::ZeroVector;
	int32 Index = 0;

	// Extract and clamp values from the BoxPoint based on enabled dimensions
	if (bHasXDimension && Index < Action.Values.Num())
	{
		Result.X = FMath::Clamp(Action.Values[Index++], MinSpeed, MaxSpeed);
	}

	if (bHasYDimension && Index < Action.Values.Num())
	{
		Result.Y = FMath::Clamp(Action.Values[Index++], MinSpeed, MaxSpeed);
	}

	if (bHasZDimension && Index < Action.Values.Num())
	{
		Result.Z = FMath::Clamp(Action.Values[Index++], MinSpeed, MaxSpeed);
	}

	return Result;
}

FString UMovementInputActuator::GenerateId() const
{
	return FString::Printf(TEXT("MovementInput_X_%s_Y_%s_Z_%s_Min_%.2f_Max_%.2f"),
		bHasXDimension ? TEXT("true") : TEXT("false"),
		bHasYDimension ? TEXT("true") : TEXT("false"),
		bHasZDimension ? TEXT("true") : TEXT("false"),
		MinSpeed,
		MaxSpeed);
}
