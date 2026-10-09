// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "Actuators/TeleportActuator.h"
#include "LogScholaInteractors.h"
#include "GameFramework/Actor.h"

void UTeleportActuator::GetActionSpace_Implementation(FInstancedStruct& OutActionSpace) const
{
    OutActionSpace.InitializeAs<FMultiDiscreteSpace>();
    FMultiDiscreteSpace& Space = OutActionSpace.GetMutable<FMultiDiscreteSpace>();
    
    if (FMath::CountBits(static_cast<uint32>(XMovementDirectionFlags)) >= 2) 
    { 
        const int32 NumXDirections = FMath::CountBits(static_cast<uint32>(XMovementDirectionFlags));
        Space.Add(NumXDirections);
    }
    if (FMath::CountBits(static_cast<uint32>(YMovementDirectionFlags)) >= 2) 
    { 
        const int32 NumYDirections = FMath::CountBits(static_cast<uint32>(YMovementDirectionFlags));
        Space.Add(NumYDirections); 
    }
    if (FMath::CountBits(static_cast<uint32>(ZMovementDirectionFlags)) >= 2) 
    { 
        const int32 NumZDirections = FMath::CountBits(static_cast<uint32>(ZMovementDirectionFlags));
        Space.Add(NumZDirections); 
    }
}

float UTeleportActuator::DimensionValue(float Step, uint8 DimensionFlags, int Digit) const
{
    // Find the Nth set bit in DimensionFlags (where N = Digit)
    int CurrentIndex = 0;
    for (int BitPos = 0; BitPos < 4; ++BitPos)
    {
        uint8 BitFlag = 1 << BitPos;
        if ((DimensionFlags & BitFlag) != 0)
        {
            if (CurrentIndex == Digit)
            {
                // Found the bit corresponding to this digit
                switch (static_cast<ETeleportDimensionFlags>(BitFlag))
                {
                    case ETeleportDimensionFlags::None:      return 0.f;
                    case ETeleportDimensionFlags::Forwards:  return Step;
                    case ETeleportDimensionFlags::Backwards: return -Step;
                    default: return 0.f;
                }
            }
            ++CurrentIndex;
        }
    }
    return 0.f;
}

FVector UTeleportActuator::ConvertActionToVector(const FMultiDiscretePoint& ActionPoint) const
{
    FVector Out(0.f);
    int Offset = 0;
    if (FMath::CountBits(static_cast<uint32>(XMovementDirectionFlags)) >= 2) { Out.X = DimensionValue(XStep, XMovementDirectionFlags, ActionPoint[Offset++]); }
    if (FMath::CountBits(static_cast<uint32>(YMovementDirectionFlags)) >= 2) { Out.Y = DimensionValue(YStep, YMovementDirectionFlags, ActionPoint[Offset++]); }
    if (FMath::CountBits(static_cast<uint32>(ZMovementDirectionFlags)) >= 2) { Out.Z = DimensionValue(ZStep, ZMovementDirectionFlags, ActionPoint[Offset++]); }
    return Out;
}

void UTeleportActuator::TakeAction(const FMultiDiscretePoint& ActionPoint)
{
    AActor* OwnerActor = GetOwner();
    if (!OwnerActor)
    {
        UE_LOGFMT(LogScholaInteractors, Warning, "UTeleportActuator::TakeAction(): No Owner to teleport - {0}", GetName());
        return;
    }
    const FVector Delta = ConvertActionToVector(ActionPoint);
    OnTeleportDelegate.Broadcast(Delta);
    OwnerActor->SetActorLocation(OwnerActor->GetActorLocation() + Delta, bSweep, nullptr, TeleportType);
}

void UTeleportActuator::TakeAction_Implementation(const FInstancedStruct& InAction)
{
    const FMultiDiscretePoint* MD = InAction.GetPtr<FMultiDiscretePoint>();
    if (!MD)
    {
        UE_LOGFMT(LogScholaInteractors, Warning, "UTeleportActuator::TakeAction_Implementation(): InAction is not a MultiDiscretePoint - {0}", GetName());
        return;
    }
    TakeAction(*MD);
}

FString UTeleportActuator::GenerateId() const
{
    FString Out = TEXT("Teleport");
    if (FMath::CountBits(static_cast<uint32>(XMovementDirectionFlags)) >= 2) { Out.Appendf(TEXT("_X_%0.2f"), XStep); }
    if (FMath::CountBits(static_cast<uint32>(YMovementDirectionFlags)) >= 2) { Out.Appendf(TEXT("_Y_%0.2f"), YStep); }
    if (FMath::CountBits(static_cast<uint32>(ZMovementDirectionFlags)) >= 2) { Out.Appendf(TEXT("_Z_%0.2f"), ZStep); }
    return Out;
}
