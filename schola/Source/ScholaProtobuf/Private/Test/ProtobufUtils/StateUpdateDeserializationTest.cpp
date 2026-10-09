// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"

#include "ProtobufUtils/ProtobufDeserializer.h"
#include "TrainingDataTypes/EnvironmentUpdate.h"
#include "TrainingDataTypes/StartRequest.h"
#include "TrainingDataTypes/TrainingUpdate.h"
#include "Points/Point.h"
#include "Points/DiscretePoint.h"

THIRD_PARTY_INCLUDES_START
#include "ScholaProtobufMacroGuardBegin.h"
#include "StateUpdates.pb.h"
#include "ScholaProtobufMacroGuardEnd.h"
THIRD_PARTY_INCLUDES_END
#include <string>

using Schola::CommunicatorStatus;

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FProtobufEnvStepDeserializationTest, "Schola.Protobuf.Deserialization.Update.EnvStep", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FProtobufEnvStepDeserializationTest::RunTest(const FString& Parameters)
{
	// Build EnvironmentStep proto with an updates map entry "agentA" -> discrete point value 4
	Schola::EnvironmentStep InProto;
	Schola::Point PointProto;
	PointProto.mutable_discrete_point()->set_value(4);
	(*InProto.mutable_updates())[std::string(TCHAR_TO_UTF8(TEXT("agentA")))] = PointProto;

	FEnvStep Out;
	ProtobufDeserializer::FromProto(InProto, Out);

	// Check that Actions contains agentA
	const TInstancedStruct<FPoint>* Found = Out.Actions.Find(TEXT("agentA"));
	TestTrue(TEXT("EnvStep actions contains 'agentA'"), Found != nullptr);
	if (Found)
	{
		const FDiscretePoint* Discrete = Found->GetPtr<FDiscretePoint>();
		TestTrue(TEXT("Agent action is discrete point"), Discrete != nullptr);
		if (Discrete)
		{
			TestEqual(TEXT("Discrete value matches 4"), Discrete->Value, 4);
		}
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FProtobufEnvResetDeserializationTest, "Schola.Protobuf.Deserialization.Update.EnvReset", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FProtobufEnvResetDeserializationTest::RunTest(const FString& Parameters)
{
	// Build EnvironmentSettings proto with options and seed
	Schola::EnvironmentSettings InProto;
	(*InProto.mutable_options())[std::string("optA")] = std::string("vA");
	InProto.set_seed(42);

	FEnvReset Out;
	ProtobufDeserializer::FromProto(InProto, Out);

	// Options check
	const FString* Val = Out.Options.Find(TEXT("optA"));
	TestTrue(TEXT("Options contains 'optA'"), Val != nullptr);
	if (Val)
	{
		TestEqual(TEXT("Options['optA'] == 'vA'"), *Val, FString(TEXT("vA")));
	}

	// Seed checks
	TestTrue(TEXT("Has seed flag set"), Out.bHasSeed);
	TestEqual(TEXT("Seed == 42"), Out.Seed, 42);

	// Also verify behavior when seed not present (bHasSeed == false)
	Schola::EnvironmentSettings NoSeedProto;
	(*NoSeedProto.mutable_options())[std::string(TCHAR_TO_UTF8(TEXT("k")))] = std::string(TCHAR_TO_UTF8(TEXT("v")));
	FEnvReset OutNoSeed;
	ProtobufDeserializer::FromProto(NoSeedProto, OutNoSeed);
	TestTrue(TEXT("No-seed proto produces bHasSeed == false"), !OutNoSeed.bHasSeed);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FProtobufStartRequestDeserializationTest, "Schola.Protobuf.Deserialization.Update.StartRequest", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FProtobufStartRequestDeserializationTest::RunTest(const FString& Parameters)
{
	// Test NEXT_STEP mapping
	Schola::GymConnectorStartRequest NextProto;
	NextProto.set_autoreset_type(Schola::AutoResetType::NEXT_STEP);

	FStartRequest NextOut;
	ProtobufDeserializer::FromProto(NextProto, NextOut);
	TestTrue(TEXT("AutoResetType NEXT_STEP -> NextStep"), NextOut.AutoResetType == EAutoResetType::NextStep);

	// Test SAME_STEP mapping
	Schola::GymConnectorStartRequest SameProto;
	SameProto.set_autoreset_type(Schola::AutoResetType::SAME_STEP);

	FStartRequest SameOut;
	ProtobufDeserializer::FromProto(SameProto, SameOut);
	TestTrue(TEXT("AutoResetType SAME_STEP -> SameStep"), SameOut.AutoResetType == EAutoResetType::SameStep);

	// Test DISABLED (or default) mapping
	Schola::GymConnectorStartRequest DisabledProto;
	DisabledProto.set_autoreset_type(Schola::AutoResetType::DISABLED);

	FStartRequest DisabledOut;
	ProtobufDeserializer::FromProto(DisabledProto, DisabledOut);
	TestTrue(TEXT("AutoResetType DISABLED -> Disabled"), DisabledOut.AutoResetType == EAutoResetType::Disabled);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FProtobufTrainingStepDeserializationTest, "Schola.Protobuf.Deserialization.Update.TrainingStep", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FProtobufTrainingStepDeserializationTest::RunTest(const FString& Parameters)
{
	// Build Step proto with one EnvironmentStep which contains an action for "a1" -> discrete value 7
	Schola::Step StepProto;
	Schola::EnvironmentStep* EnvStepProto = StepProto.add_environments();
	Schola::Point P;
	P.mutable_discrete_point()->set_value(7);
	(*EnvStepProto->mutable_updates())[std::string("a1")] = P;

	FTrainingStep OutStep;
	ProtobufDeserializer::FromProto(StepProto, OutStep);

	TestEqual(TEXT("Step EnvSteps count == 1"), OutStep.EnvSteps.Num(), 1);
	if (OutStep.EnvSteps.Num() == 1)
	{
		const TInstancedStruct<FPoint>* Found = OutStep.EnvSteps[0].Actions.Find(TEXT("a1"));
		TestTrue(TEXT("EnvStep[0] contains action a1"), Found != nullptr);
		if (Found)
		{
			const FDiscretePoint* Discrete = Found->GetPtr<FDiscretePoint>();
			TestTrue(TEXT("Action deserializes to discrete point"), Discrete != nullptr);
			if (Discrete)
			{
				TestEqual(TEXT("Discrete value == 7"), Discrete->Value, 7);
			}
		}
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FProtobufTrainingResetDeserializationTest, "Schola.Protobuf.Deserialization.Update.Reset", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FProtobufTrainingResetDeserializationTest::RunTest(const FString& Parameters)
{
	// Build Reset proto with environments map<int, EnvironmentResetSettings> key 3
	Schola::Reset					 ResetProto;
	Schola::EnvironmentSettings EnvResetProto;
	(*EnvResetProto.mutable_options())[std::string("o")] = std::string("v");
	EnvResetProto.set_seed(123);
	(*ResetProto.mutable_environments())[3] = EnvResetProto;

	FTrainingReset OutReset;
	ProtobufDeserializer::FromProto(ResetProto, OutReset);

	TestTrue(TEXT("Reset contains key 3"), OutReset.Environments.Contains(3));
	if (OutReset.Environments.Contains(3))
	{
		const FEnvReset& E = OutReset.Environments[3];
		const FString*	 OptVal = E.Options.Find(TEXT("o"));
		TestTrue(TEXT("Reset options contains 'o'"), OptVal != nullptr);
		if (OptVal)
		{
			TestEqual(TEXT("Reset option 'o' == 'v'"), *OptVal, FString(TEXT("v")));
		}
		TestTrue(TEXT("Reset has seed flag true"), E.bHasSeed);
		TestEqual(TEXT("Reset seed == 123"), E.Seed, 123);
	}

	return true;
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FProtobufTrainingStateUpdateDeserializationTest, "Schola.Protobuf.Deserialization.Update.TrainingStateUpdate", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FProtobufTrainingStateUpdateDeserializationTest::RunTest(const FString& Parameters)
{
	// Case A: Step
	Schola::StateUpdate StepStateProto;
	StepStateProto.set_status(Schola::CommunicatorStatus::GOOD);
	Schola::Step* InnerStep = StepStateProto.mutable_step();
	Schola::EnvironmentStep* EnvStepProto = InnerStep->add_environments();
	Schola::Point P;
	P.mutable_discrete_point()->set_value(11);
	(*EnvStepProto->mutable_updates())[std::string("agentX")] = P;

	FTrainingStateUpdate OutState;
	ProtobufDeserializer::FromProto(StepStateProto, OutState);

	TestTrue(TEXT("StateUpdate is step"), OutState.IsStep());
	TestTrue(TEXT("StateUpdate not reset"), !OutState.IsReset());
	TestTrue(TEXT("Status mapped to NONE"), OutState.Status == EConnectorStatusUpdate::NONE);
	if (OutState.IsStep())
	{
		const FTrainingStep& StepRef = OutState.GetStep();
		TestEqual(TEXT("Step.EnvSteps.Num == 1"), StepRef.EnvSteps.Num(), 1);
		if (StepRef.EnvSteps.Num() == 1)
		{
			const TInstancedStruct<FPoint>* Found = StepRef.EnvSteps[0].Actions.Find(TEXT("agentX"));
			TestTrue(TEXT("agentX present"), Found != nullptr);
			if (Found)
			{
				const FDiscretePoint* Discrete = Found->GetPtr<FDiscretePoint>();
				TestTrue(TEXT("agentX deserializes to discrete"), Discrete != nullptr);
				if (Discrete)
				{
					TestEqual(TEXT("agentX value == 11"), Discrete->Value, 11);
				}
			}
		}
	}

	// Case B: Reset
	Schola::StateUpdate ResetStateProto;
	ResetStateProto.set_status(Schola::CommunicatorStatus::GOOD);
	Schola::Reset* InnerReset = ResetStateProto.mutable_reset();
	Schola::EnvironmentSettings ES;
	(*ES.mutable_options())[std::string("x")] = std::string("y");
	ES.set_seed(7);
	(*InnerReset->mutable_environments())[5] = ES;

	FTrainingStateUpdate OutResetState;
	ProtobufDeserializer::FromProto(ResetStateProto, OutResetState);

	TestTrue(TEXT("StateUpdate is reset"), OutResetState.IsReset());
	TestTrue(TEXT("StateUpdate not step"), !OutResetState.IsStep());
	TestTrue(TEXT("StatusUpdate not error"), !OutResetState.IsError());
	if (OutResetState.IsReset())
	{
		const FTrainingReset& R = OutResetState.GetReset();
		TestTrue(TEXT("Reset contains key 5"), R.Environments.Contains(5));
		if (R.Environments.Contains(5))
		{
			const FEnvReset& E = R.Environments[5];
			const FString* Opt = E.Options.Find(TEXT("x"));
			TestTrue(TEXT("Reset options contains 'x'"), Opt != nullptr);
			if (Opt)
			{
				TestEqual(TEXT("Reset option x == y"), *Opt, FString("y"));
			}
			TestTrue(TEXT("Reset seed flag true"), E.bHasSeed);
			TestEqual(TEXT("Reset seed == 7"), E.Seed, 7);
		}
	}
	
	//Case C: Error
	Schola::StateUpdate ErrorStateProto;
	FTrainingStateUpdate OutErrorState;
	ErrorStateProto.set_status(static_cast<Schola::CommunicatorStatus>(1));

	ProtobufDeserializer::FromProto(ErrorStateProto, OutErrorState);
	TestTrue(TEXT("StatusUpdate is error"), OutErrorState.IsError());
	TestTrue(TEXT("StatusUpdate is not step"), !OutErrorState.IsStep());
	TestTrue(TEXT("StatusUpdate is not reset"), !OutErrorState.IsReset());

	return true;
}
