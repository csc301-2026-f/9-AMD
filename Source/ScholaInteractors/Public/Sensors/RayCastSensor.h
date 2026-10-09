// Copyright (c) 2023-2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/HitResult.h"
#include "SensorInterface.h"
#include "RayCastSensor.generated.h"

/**
 * @brief Raycast sensor that collects observations by casting rays into the environment.
 * 
 * This sensor fires multiple rays in a cone or full circle pattern, detecting objects
 * and measuring distances. For each ray, it reports whether an object was hit, the hit distance,
 * and which tracked tags (if any) the hit object has. Useful for navigation and obstacle detection.
 */
UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class SCHOLAINTERACTORS_API URayCastSensor : public USceneComponent, public IScholaSensor
{
	GENERATED_BODY()

	static constexpr const float kLineGirth = 1.f;      /** Line thickness for debug visualization */
	static constexpr const float kSphereRadius = 16.f;  /** Sphere radius for debug visualization at hit points */

public:
	
    /** The baseline length of each ray. Will be adjusted by the scale component of RayTransform. */
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Sensor Properties", meta = (ClampMin = "1"))
	float RayLength = 4096.f;

	/** The collision channel to use for the raycast. Determines what types of objects the rays can hit. */
	UPROPERTY(EditAnywhere, Category = "Sensor Properties|Trace Options")
	TEnumAsByte<ECollisionChannel> CollisionChannel;

	/** Does this sensor draw debug lines/spheres during runtime. Useful for visualizing ray directions and hits. */
	UPROPERTY(EditAnywhere, Category = "Sensor Properties")
	bool bDrawDebugLines = false;

	/** Should the sensor trace against complex collision. If false, uses simple collision shapes. */
	UPROPERTY(EditAnywhere, Category = "Sensor Properties|Trace Options")
	bool bTraceComplex = false;

	/** The number of rays to fire. Rays are distributed evenly across RayDegrees. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sensor Properties", meta = (ClampMin = "1"))
	int32 NumRays = 2;

	/** The angle between the first and last ray. Special case of 360 degrees creates a full circle. */
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Sensor Properties", meta = (ClampMin = "0", ClampMax = "360"))
	float RayDegrees = 90.0f;

	/** Actor tags that are checked on raycast collision. Included in observations as a 1-hot vector per ray. */
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Sensor Properties")
	TArray<FName> TrackedTags;

	/** A position adjustment that is applied to end points of the generated ray trajectories. Useful for fine-tuning ray placement. */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta=(MakeEditWidget), Category = "Sensor Properties")
	FVector RayEndOffset;

	/** Debug color for ray hit (when ray intersects an object). */
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Sensor Properties")
	FColor DebugHitColor = FColor::Green;

	/** Debug color for ray miss (when ray hits nothing). */
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Sensor Properties")
	FColor DebugMissColor = FColor::Red;

	/** Should the sensor run raytraces in parallel. */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta=(InlineEditConditionToggle), Category = "Sensor Properties")
	bool bEnableParallelTracing = false;

	/** The minimum number of rays to process in each parallel batch. If bigger than the number of rays parallelization may be disabled.*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite, meta = (ClampMin = "1", EditCondition = "bEnableParallelTracing"), Category = "Sensor Properties")
	int32 MinParallelBatchSize = 1;

	/**
	 * @brief Get the observation space for this sensor.
	 * 
	 * Returns a BoxSpace with dimensions for each ray's observation data:
	 * - One dimension per tracked tag (1-hot encoded)
	 * - One dimension for hit/miss (1.0 if hit, 0.0 if miss)
	 * - One dimension for normalized hit distance (0.0 to 1.0)
	 * All dimensions are bounded [0.0, 1.0].
	 * 
	 * @param[out] OutObservationSpace The observation space definition to be populated
	 */
	virtual void GetObservationSpace_Implementation(FInstancedStruct& OutObservationSpace) const override;

	/**
	 * @brief Generate the endpoints of the rays to be cast.
	 * 
	 * Distributes ray endpoints evenly across the specified angle range.
	 * Handles special case of 360 degrees to avoid overlapping rays.
	 * Applies transform and offset to position rays in world space.
	 * 
	 * @param[in] InNumRays The number of rays to generate
	 * @param[in] InRayDegrees The angle between the first and last ray
	 * @param[in] InBaseEnd The base endpoint of the rays before rotation
	 * @param[in] InStart The start point for all rays
	 * @param[in] InBaseTransform A transform to apply to the generated endpoints
	 * @param[in] InEndOffset An offset to apply to the end points of the rays
	 * @param[out] OutRayEndpoints Array to be populated with ray endpoint positions
	 */
	void GenerateRayEndpoints(int32 InNumRays, float InRayDegrees, FVector InBaseEnd,FVector InStart, FTransform InBaseTransform, FVector InEndOffset, TArray<FVector>& OutRayEndpoints);

	/**
	 * @brief Add empty tags to the observation for rays that miss.
	 * 
	 * Appends zeros for each tracked tag, representing no tag matches.
	 * 
	 * @param[in,out] OutObservations The observations to append the empty tag values to
	 */
	void AppendEmptyTags(FBoxPoint& OutObservations);

	/**
	 * @brief Handle a successful ray trace and append observation data.
	 * 
	 * Checks the hit actor's tags against TrackedTags and appends a 1-hot encoding.
	 * Appends hit flag (1.0) and normalized hit distance.
	 * Optionally draws debug visualization.
	 * 
	 * @param[in] InHitResult The result of the ray trace containing hit information
	 * @param[in,out] OutObservations The observations to append the results to
	 * @param[in] InStart The start point of the ray
	 */
	static void HandleRayHit(const UWorld* World, const TArray<FName>& TrackedTags, const FHitResult& InHitResult, TArrayView<float>& OutObservations, const FVector& InStart);

	/**
	 * @brief Collect observations about the environment state by casting rays.
	 * 
	 * Generates ray endpoints, performs line traces, and processes hit/miss results.
	 * The resulting BoxPoint contains flattened data for all rays.
	 * 
	 * @param[out] OutObservations A BoxPoint that will be populated with ray sensor data
	 */
	virtual void CollectObservations_Implementation(FInstancedStruct& OutObservations) override;

	/**
	 * @brief Generate a unique ID string for this sensor.
	 * 
	 * @return FString describing the sensor configuration (e.g., "Ray_Num_8_Deg_90.00_Max_4096.00_ECC_Camera_Tags_Enemy_Wall")
	 */
	FString GenerateId() const;

#if WITH_EDITORONLY_DATA
	/** Should we draw debug lines in the editor. */
	UPROPERTY()
	bool bDebugLinesEnabled = false;
#endif

#if WITH_EDITOR
	/**
	 * @brief Draw persistent debug lines showing ray directions and endpoints.
	 * 
	 * Used in editor for visualizing sensor configuration.
	 */
	void DrawDebugLines();

	/**
	 * @brief Toggle debug line visualization on/off.
	 * 
	 * Can be called from editor UI to enable/disable visualization.
	 */
	UFUNCTION(CallInEditor, Category = "Sensor Utilities")
	void ToggleDebugLines();
#endif
};
