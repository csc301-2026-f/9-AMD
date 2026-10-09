// The below code is adapted from https://github.com/DLR-RM/stable-baselines3/blob/master/tests/test_vec_envs.py

/* 
The MIT License

Copyright (c) 2019 Antonin Raffin

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

HE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

// Modifications Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "Environment/EnvironmentInterface.h"
#include "Environment/MultiAgentEnvironmentInterface.h"
#include "UObject/Object.h"
#include "Environment/MultiAgentEnvironmentInterface.h"
#include "StepEnv.generated.h"


UCLASS(BlueprintType)
class UStepEnv : public UObject, public IMultiAgentScholaEnvironment
{
    GENERATED_BODY()

public:
    UStepEnv(const FObjectInitializer& ObjectInitializer);

    // IScholaEnvironment overrides
    void InitializeEnvironment_Implementation(TMap<FString, FInteractionDefinition>& OutAgentDefinitions);

    void SeedEnvironment_Implementation(int Seed) {}

    void SetEnvironmentOptions_Implementation(const TMap<FString, FString>& InOptions) {}

    void Reset_Implementation(TMap<FString, FInitialAgentState>& OutAgentState);

    void Step_Implementation(const TMap<FString, FInstancedStruct>& InActions, TMap<FString, FAgentState>& OutAgentStates);

    // Expose current logical step for tests
    int GetCurrentStep() const { return CurrentStep; }

    /** Maximum number of steps before the environment is considered truncated/completed. Editable in editor/blueprints. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="StepEnv", meta=(AllowPrivateAccess="true"))
    int MaxSteps = 1;

    /** Current step count (visible in editor / Blueprints). */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="StepEnv", meta=(AllowPrivateAccess="true"))
    int CurrentStep = 0;

    UPROPERTY()
    FString AgentName = TEXT("StepAgent");
private:
   

    void BuildObservation(TInstancedStruct<FPoint>& OutObservation) const;
};
