// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"

/**
 * @brief Reinterpret a typed TInstancedStruct<T> as its type-erased FInstancedStruct base.
 *
 * TInstancedStruct<T> is a compile-time wrapper around FInstancedStruct with the same
 * memory layout, so a reference cast is safe and free. Use this to bridge Schola's
 * typed C++ APIs to UFunction/Blueprint surfaces that take FInstancedStruct.
 *
 * The returned reference aliases the argument, so rvalue overloads are deleted to
 * prevent callers from binding the result to a reference that outlives a temporary.
 *
 * @tparam T The struct type held by the instanced struct.
 * @param[in] InStruct The typed instanced struct to reinterpret.
 * @return A reference to InStruct reinterpreted as FInstancedStruct. The reference
 *         is valid for the lifetime of InStruct.
 */
template<typename T>
FORCEINLINE FInstancedStruct& ToUntypedInstancedStruct(TInstancedStruct<T>& InStruct)
{
	return reinterpret_cast<FInstancedStruct&>(InStruct);
}

template<typename T>
FORCEINLINE const FInstancedStruct& ToUntypedInstancedStruct(const TInstancedStruct<T>& InStruct)
{
	return reinterpret_cast<const FInstancedStruct&>(InStruct);
}

template<typename T>
void ToUntypedInstancedStruct(TInstancedStruct<T>&&) = delete;

template<typename T>
void ToUntypedInstancedStruct(const TInstancedStruct<T>&&) = delete;

namespace InstancedStructUtils::Private
{
	template<typename T>
	FORCEINLINE void CheckTypeCompatibility(const FInstancedStruct& InStruct)
	{
		checkf(!InStruct.IsValid() || InStruct.GetScriptStruct()->IsChildOf(T::StaticStruct()),
			TEXT("ToTypedInstancedStruct: expected %s but got %s"),
			*T::StaticStruct()->GetName(),
			*InStruct.GetScriptStruct()->GetName());
	}
}

/**
 * @brief Reinterpret a FInstancedStruct as a typed TInstancedStruct<T>.
 *
 * If InStruct is valid (i.e. has both a script struct and allocated memory), a
 * debug-only checkf verifies the contained type is a T (or subclass), compiled
 * out in shipping builds. Invalid/uninitialized structs pass through unchecked
 * — the callee is expected to populate them.
 *
 * The returned reference aliases the argument, so rvalue overloads are deleted to
 * prevent callers from binding the result to a reference that outlives a temporary.
 *
 * @tparam T The expected struct type. Must be the same as, or a base of, the type held by InStruct.
 * @param[in] InStruct The type-erased instanced struct to reinterpret.
 * @return A reference to InStruct reinterpreted as TInstancedStruct<T>. The reference
 *         is valid for the lifetime of InStruct.
 */
template<typename T>
FORCEINLINE TInstancedStruct<T>& ToTypedInstancedStruct(FInstancedStruct& InStruct)
{
	InstancedStructUtils::Private::CheckTypeCompatibility<T>(InStruct);
	return reinterpret_cast<TInstancedStruct<T>&>(InStruct);
}

template<typename T>
FORCEINLINE const TInstancedStruct<T>& ToTypedInstancedStruct(const FInstancedStruct& InStruct)
{
	InstancedStructUtils::Private::CheckTypeCompatibility<T>(InStruct);
	return reinterpret_cast<const TInstancedStruct<T>&>(InStruct);
}

template<typename T>
void ToTypedInstancedStruct(FInstancedStruct&&) = delete;

template<typename T>
void ToTypedInstancedStruct(const FInstancedStruct&&) = delete;
