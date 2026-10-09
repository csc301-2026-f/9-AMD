// Copyright (c) 2023-2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once

#include "Points/Point.h"
#include "Points/PointVisitor.h"
#include "DiscretePoint.generated.h"

/**
 * @struct FDiscretePoint
 * @brief A point in a discrete space with a single integer value.
 * 
 * Discrete points represent a single choice from a finite set of options,
 * identified by an integer index. Commonly used for discrete actions
 * like button presses or menu selections.
 */
USTRUCT(BlueprintType)
struct SCHOLA_API FDiscretePoint : public FPoint
{
	GENERATED_BODY()
	
	/**
	 * @brief The integer value of this point.
	 */
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Point")
	int Value = 0;

	/**
	 * @brief Constructs an empty DiscretePoint with value 0.
	 */
	FDiscretePoint()
		: Value(0)
	{
	}

	/**
	 * @brief Constructs a DiscretePoint with a specific integer value.
	 * @param[in] Value The integer value to initialize the point with.
	 */
	FDiscretePoint(int Value)
		: Value(Value)
	{
	}

	/**
	 * @brief Virtual destructor.
	 */
	virtual ~FDiscretePoint()
	{
	}

	/**
	 * @brief Accepts a mutable visitor for the visitor pattern.
	 * @param[in,out] Visitor The visitor to accept.
	 */
	void Accept(PointVisitor& Visitor) override;

	/**
	 * @brief Accepts a const visitor for the visitor pattern.
	 * @param[in,out] Visitor The const visitor to accept.
	 */
	void Accept(ConstPointVisitor& Visitor) const override;

	/**
	 * @brief Resets the value of the DiscretePoint to 0.
	 */
	void Reset() override
	{
		this->Value = 0;
	};

	/**
	 * @brief Converts this point to a string representation.
	 * @return A string containing the integer value.
	 */
	FString ToString() const override
	{
		return FString::Printf(TEXT("%d"), this->Value);
	}
};
