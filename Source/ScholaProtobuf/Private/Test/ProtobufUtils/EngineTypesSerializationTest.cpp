// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"

#include "ProtobufUtils/ProtobufSerializer.h"

#include <string>

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FProtobufFStringToStdStringTest, "Schola.Protobuf.Serialization.FStringToStdString", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FProtobufFStringToStdStringTest::RunTest(const FString& Parameters)
{
	FString In = TEXT("hello world");
	std::string Out;
	ProtobufSerializer::ToProto(In, &Out);

	const std::string Expected = TCHAR_TO_UTF8(*In);
	TestTrue(TEXT("Converted std::string should match UTF-8 of FString"), Out == Expected);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FProtobufTMapFStringToProtoTest, "Schola.Protobuf.Serialization.Maps.TMapFStringFString", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FProtobufTMapFStringToProtoTest::RunTest(const FString& Parameters)
{
	TMap<FString, FString> UnrealMap;
	UnrealMap.Add(TEXT("alpha"), TEXT("one"));
	UnrealMap.Add(TEXT("beta"), TEXT("two"));

	Schola::AgentState Proto; // use agent state info map as a target
	ProtobufSerializer::ToProto(UnrealMap, Proto.mutable_info());

	const auto& Map = Proto.info();
	auto ItA = Map.find(std::string(TCHAR_TO_UTF8(TEXT("alpha"))));
	auto ItB = Map.find(std::string(TCHAR_TO_UTF8(TEXT("beta"))));

	TestTrue(TEXT("alpha present in proto map"), ItA != Map.end());
	TestTrue(TEXT("beta present in proto map"), ItB != Map.end());
	if (ItA != Map.end()) TestTrue(TEXT("alpha == one"), ItA->second == std::string(TCHAR_TO_UTF8(TEXT("one"))));
	if (ItB != Map.end()) TestTrue(TEXT("beta == two"), ItB->second == std::string(TCHAR_TO_UTF8(TEXT("two"))));

	return true;
}
