// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

// Deliberately no include guard: this file is used as an include-pair wrapper.
// It must be processed every time it is included so that the matching pop_macro
// runs once per third-party include block. Adding #pragma once or include guards
// will silently break the verify-macro sandboxing.
//
// Pair this file with ScholaProtobufMacroGuardBegin.h. The sentinel macro
// SCHOLA_PROTOBUF_VERIFY_MACRO_PUSHED ensures the pop only runs when the
// matching push actually ran (i.e. when `verify` was defined at the include
// site), keeping the macro stack balanced.

#ifdef SCHOLA_PROTOBUF_VERIFY_MACRO_PUSHED
#pragma pop_macro("verify")
#undef SCHOLA_PROTOBUF_VERIFY_MACRO_PUSHED
#endif
