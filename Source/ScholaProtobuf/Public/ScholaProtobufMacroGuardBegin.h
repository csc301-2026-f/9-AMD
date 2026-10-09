// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

// Deliberately no include guard: this file is used as an include-pair wrapper.
// It must be processed every time it is included so that the push_macro / undef
// runs once per third-party include block. Adding #pragma once or include guards
// will silently break the verify-macro sandboxing.
//
// Pair this file with ScholaProtobufMacroGuardEnd.h around any include block
// that pulls in protobuf / gRPC / Abseil headers. Unreal defines `verify` as a
// macro, while Abseil's btree containers use `verify` as a member function name.
// We push and undef the macro here so the third-party headers parse correctly,
// and the End file restores it via pop_macro.

#ifdef verify
#pragma push_macro("verify")
#undef verify
#define SCHOLA_PROTOBUF_VERIFY_MACRO_PUSHED 1
#endif
