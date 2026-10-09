// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"

#include "ImitationUtils/EnhancedInputUtils.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "StructUtils/InstancedStruct.h"
#include "Spaces/DictSpace.h"
#include "Spaces/BoxSpace.h"
#include "Spaces/MultiBinarySpace.h"
#include "UObject/Package.h"

#if WITH_DEV_AUTOMATION_TESTS

namespace
{
	// Helper to create a named UInputAction
	UInputAction* CreateInputAction(const FName& Name, EInputActionValueType ValueType)
	{
		UInputAction* Action = NewObject<UInputAction>(static_cast<UObject*>(GetTransientPackage()), UInputAction::StaticClass(), Name);
		Action->ValueType = ValueType;
		return Action;
	}
} // namespace

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBuildActionSpaceFromIMC_Boolean, "Schola.Imitation.EnhancedInput.BuildActionSpace.Boolean", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FBuildActionSpaceFromIMC_Boolean::RunTest(const FString& Parameters)
{
	UInputAction*		  Action = CreateInputAction(TEXT("IA_Jump"), EInputActionValueType::Boolean);
	UInputMappingContext* IMC = NewObject<UInputMappingContext>();
	IMC->MapKey(Action, EKeys::SpaceBar);

	TInstancedStruct<FSpace> OutSpace;
	TestTrue(TEXT("BuildActionSpaceFromIMC succeeded"), FEnhancedInputUtils::BuildActionSpaceFromIMC(IMC, OutSpace));

	if (const FDictSpace* Dict = OutSpace.GetPtr<FDictSpace>())
	{
		TestEqual(TEXT("Entry count"), Dict->Spaces.Num(), 1);
		for (const auto& Pair : Dict->Spaces)
		{
			TestTrue(TEXT("Is MultiBinarySpace"), Pair.Value.GetScriptStruct() == FMultiBinarySpace::StaticStruct());
			if (const auto* MB = Pair.Value.GetPtr<FMultiBinarySpace>())
			{
				TestEqual(TEXT("Shape"), MB->Shape, 1);
			}
		}
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBuildActionSpaceFromIMC_Axis1D, "Schola.Imitation.EnhancedInput.BuildActionSpace.Axis1D", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FBuildActionSpaceFromIMC_Axis1D::RunTest(const FString& Parameters)
{
	UInputAction*		  Action = CreateInputAction(TEXT("IA_Throttle"), EInputActionValueType::Axis1D);
	UInputMappingContext* IMC = NewObject<UInputMappingContext>();
	IMC->MapKey(Action, EKeys::W);

	TInstancedStruct<FSpace> OutSpace;
	FEnhancedInputUtils::BuildActionSpaceFromIMC(IMC, OutSpace);

	if (const FDictSpace* Dict = OutSpace.GetPtr<FDictSpace>())
	{
		TestEqual(TEXT("Entry count"), Dict->Spaces.Num(), 1);
		for (const auto& Pair : Dict->Spaces)
		{
			TestTrue(TEXT("Is BoxSpace"), Pair.Value.GetScriptStruct() == FBoxSpace::StaticStruct());
			if (const auto* Box = Pair.Value.GetPtr<FBoxSpace>())
			{
				TestEqual(TEXT("Dimensions"), Box->Dimensions.Num(), 1);
				TestTrue(TEXT("Low"), FMath::IsNearlyEqual(Box->Dimensions[0].Low, -1.0f));
				TestTrue(TEXT("High"), FMath::IsNearlyEqual(Box->Dimensions[0].High, 1.0f));
			}
		}
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBuildActionSpaceFromIMC_Axis2D, "Schola.Imitation.EnhancedInput.BuildActionSpace.Axis2D", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FBuildActionSpaceFromIMC_Axis2D::RunTest(const FString& Parameters)
{
	UInputAction*		  Action = CreateInputAction(TEXT("IA_Move"), EInputActionValueType::Axis2D);
	UInputMappingContext* IMC = NewObject<UInputMappingContext>();
	IMC->MapKey(Action, EKeys::Gamepad_LeftStick_Up);

	TInstancedStruct<FSpace> OutSpace;
	FEnhancedInputUtils::BuildActionSpaceFromIMC(IMC, OutSpace);

	if (const FDictSpace* Dict = OutSpace.GetPtr<FDictSpace>())
	{
		TestEqual(TEXT("Entry count"), Dict->Spaces.Num(), 1);
		for (const auto& Pair : Dict->Spaces)
		{
			TestTrue(TEXT("Is BoxSpace"), Pair.Value.GetScriptStruct() == FBoxSpace::StaticStruct());
			if (const auto* Box = Pair.Value.GetPtr<FBoxSpace>())
			{
				TestEqual(TEXT("Dimensions"), Box->Dimensions.Num(), 2);
			}
		}
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBuildActionSpaceFromIMC_Axis3D, "Schola.Imitation.EnhancedInput.BuildActionSpace.Axis3D", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FBuildActionSpaceFromIMC_Axis3D::RunTest(const FString& Parameters)
{
	UInputAction*		  Action = CreateInputAction(TEXT("IA_Rotate"), EInputActionValueType::Axis3D);
	UInputMappingContext* IMC = NewObject<UInputMappingContext>();
	IMC->MapKey(Action, EKeys::Gamepad_RightStick_Up);

	TInstancedStruct<FSpace> OutSpace;
	FEnhancedInputUtils::BuildActionSpaceFromIMC(IMC, OutSpace);

	if (const FDictSpace* Dict = OutSpace.GetPtr<FDictSpace>())
	{
		TestEqual(TEXT("Entry count"), Dict->Spaces.Num(), 1);
		for (const auto& Pair : Dict->Spaces)
		{
			TestTrue(TEXT("Is BoxSpace"), Pair.Value.GetScriptStruct() == FBoxSpace::StaticStruct());
			if (const auto* Box = Pair.Value.GetPtr<FBoxSpace>())
			{
				TestEqual(TEXT("Dimensions"), Box->Dimensions.Num(), 3);
			}
		}
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBuildActionSpaceFromIMC_Combined, "Schola.Imitation.EnhancedInput.BuildActionSpace.Combined", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FBuildActionSpaceFromIMC_Combined::RunTest(const FString& Parameters)
{
	UInputAction* Jump = CreateInputAction(TEXT("IA_Jump"), EInputActionValueType::Boolean);
	UInputAction* Throttle = CreateInputAction(TEXT("IA_Throttle"), EInputActionValueType::Axis1D);
	UInputAction* Move = CreateInputAction(TEXT("IA_Move"), EInputActionValueType::Axis2D);

	UInputMappingContext* IMC = NewObject<UInputMappingContext>();
	IMC->MapKey(Jump, EKeys::SpaceBar);
	IMC->MapKey(Throttle, EKeys::W);
	IMC->MapKey(Move, EKeys::Gamepad_LeftStick_Up);

	TInstancedStruct<FSpace> OutSpace;
	FEnhancedInputUtils::BuildActionSpaceFromIMC(IMC, OutSpace);

	if (const FDictSpace* Dict = OutSpace.GetPtr<FDictSpace>())
	{
		TestEqual(TEXT("Entry count"), Dict->Spaces.Num(), 3);

		int MultiBinaryCount = 0;
		int BoxSpace1DCount = 0;
		int BoxSpace2DCount = 0;

		for (const auto& Pair : Dict->Spaces)
		{
			if (Pair.Value.GetScriptStruct() == FMultiBinarySpace::StaticStruct())
			{
				MultiBinaryCount++;
			}
			else if (Pair.Value.GetScriptStruct() == FBoxSpace::StaticStruct())
			{
				if (const auto* Box = Pair.Value.GetPtr<FBoxSpace>())
				{
					if (Box->Dimensions.Num() == 1)
						BoxSpace1DCount++;
					else if (Box->Dimensions.Num() == 2)
						BoxSpace2DCount++;
				}
			}
		}

		TestEqual(TEXT("MultiBinarySpace count"), MultiBinaryCount, 1);
		TestEqual(TEXT("BoxSpace 1D count"), BoxSpace1DCount, 1);
		TestEqual(TEXT("BoxSpace 2D count"), BoxSpace2DCount, 1);
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBuildActionSpaceFromIMC_DuplicateMappings, "Schola.Imitation.EnhancedInput.BuildActionSpace.DuplicateMappings", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FBuildActionSpaceFromIMC_DuplicateMappings::RunTest(const FString& Parameters)
{
	UInputAction* Action = CreateInputAction(TEXT("IA_Jump"), EInputActionValueType::Boolean);

	UInputMappingContext* IMC = NewObject<UInputMappingContext>();
	IMC->MapKey(Action, EKeys::SpaceBar);
	IMC->MapKey(Action, EKeys::Gamepad_FaceButton_Bottom);
	IMC->MapKey(Action, EKeys::W);

	TInstancedStruct<FSpace> OutSpace;
	FEnhancedInputUtils::BuildActionSpaceFromIMC(IMC, OutSpace);

	if (const FDictSpace* Dict = OutSpace.GetPtr<FDictSpace>())
	{
		TestEqual(TEXT("Entry count (deduplicated)"), Dict->Spaces.Num(), 1);
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBuildActionSpaceFromIMC_EmptyIMC, "Schola.Imitation.EnhancedInput.BuildActionSpace.EmptyIMC", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FBuildActionSpaceFromIMC_EmptyIMC::RunTest(const FString& Parameters)
{
	UInputMappingContext* IMC = NewObject<UInputMappingContext>();

	TInstancedStruct<FSpace> OutSpace;
	TestTrue(TEXT("Returns true"), FEnhancedInputUtils::BuildActionSpaceFromIMC(IMC, OutSpace));

	if (const FDictSpace* Dict = OutSpace.GetPtr<FDictSpace>())
	{
		TestEqual(TEXT("Entry count"), Dict->Spaces.Num(), 0);
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBuildActionSpaceFromIMC_NullIMC, "Schola.Imitation.EnhancedInput.BuildActionSpace.NullIMC", EAutomationTestFlags_ApplicationContextMask | EAutomationTestFlags::ProductFilter)
bool FBuildActionSpaceFromIMC_NullIMC::RunTest(const FString& Parameters)
{
	TInstancedStruct<FSpace> OutSpace;
	AddExpectedError(TEXT("LogScholaImitation: .*"), EAutomationExpectedErrorFlags::Contains, 1);
	TestFalse(TEXT("Returns false for null"), FEnhancedInputUtils::BuildActionSpaceFromIMC(nullptr, OutSpace));

	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
