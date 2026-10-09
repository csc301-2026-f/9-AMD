// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "Misc/AutomationTest.h"

#include "ProtobufUtils/ProtobufDeserializer.h"

#include <string>

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FProtobufStdStringToFStringDeserializationTest, "Schola.Protobuf.Deserialization.FStringToStdString", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FProtobufStdStringToFStringDeserializationTest::RunTest(const FString& Parameters)
{
	std::string In = std::string(TCHAR_TO_UTF8(*FString(TEXT("hello world"))));
	FString Out;
	ProtobufDeserializer::FromProto(In, Out);

	const FString Expected = FString(TEXT("hello world"));
	TestTrue(TEXT("Converted FString should match original"), Out == Expected);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FProtobufProtoMapToTMapFStringDeserializationTest, "Schola.Protobuf.Deserialization.Maps.ProtoMapToTMapFStringFString", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter)
bool FProtobufProtoMapToTMapFStringDeserializationTest::RunTest(const FString& Parameters)
{
	Schola::AgentState Proto;
	(*Proto.mutable_info())[std::string(TCHAR_TO_UTF8(TEXT("alpha")))] = std::string(TCHAR_TO_UTF8(TEXT("one")));
	(*Proto.mutable_info())[std::string(TCHAR_TO_UTF8(TEXT("beta")))] = std::string(TCHAR_TO_UTF8(TEXT("two")));

	TMap<FString, FString> OutMap;
	ProtobufDeserializer::FromProto(Proto.info(), OutMap);

	auto ItA = OutMap.Find(TEXT("alpha"));
	auto ItB = OutMap.Find(TEXT("beta"));

	TestTrue(TEXT("alpha present in TMap"), ItA != nullptr);
	TestTrue(TEXT("beta present in TMap"), ItB != nullptr);
	if (ItA) TestTrue(TEXT("alpha == one"), *ItA == TEXT("one"));
	if (ItB) TestTrue(TEXT("beta == two"), *ItB == TEXT("two"));

	return true;
}
