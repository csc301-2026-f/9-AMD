// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "ImitationSettings/ImitationSettings.h"
#include "ImitationSettings/ImitationLoggingSettings.h"
#include "TrainingUtils/ArgBuilder.h"
#include "MinariCollectionSettings.generated.h"

/**
 * @brief Configuration struct for Minari dataset collection.
 * @details Contains all settings necessary to collect an imitation learning dataset using Minari.
 * This is a Minari-specific implementation of imitation learning settings.
 */
USTRUCT(BlueprintType)
struct SCHOLAIMITATION_API FMinariCollectionSettings : public FImitationSettings
{
	GENERATED_BODY()

public:
	/** Unique identifier for the Minari dataset. This will be used to name the dataset when it is created. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Minari Collection Settings")
	FString DatasetId = TEXT("my-dataset-v0");

	/** Total number of steps to collect for the dataset. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "1"), Category = "Minari Collection Settings")
	int32 NumSteps = 1000;

	/** Random seed for reproducibility. If negative, uses a random seed. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Minari Collection Settings")
	int32 Seed = -1;

	/** Author name for the dataset metadata. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (InlineEditConditionToggle), Category = "Minari Collection Settings|Metadata")
	bool bIncludeAuthor = false;

	/** Author name for the dataset metadata. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bIncludeAuthor"), Category = "Minari Collection Settings|Metadata")
	FString Author;

	/** Author email for the dataset metadata. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (InlineEditConditionToggle), Category = "Minari Collection Settings|Metadata")
	bool bIncludeAuthorEmail = false;

	/** Author email for the dataset metadata. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bIncludeAuthorEmail"), Category = "Minari Collection Settings|Metadata")
	FString AuthorEmail;

	/** URL to the code or repository used to generate the dataset. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (InlineEditConditionToggle), Category = "Minari Collection Settings|Metadata")
	bool bIncludeCodePermalink = false;

	/** URL to the code or repository used to generate the dataset. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bIncludeCodePermalink"), Category = "Minari Collection Settings|Metadata")
	FString CodePermalink;

	/** Name of the algorithm or policy used to collect the data. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (InlineEditConditionToggle), Category = "Minari Collection Settings|Metadata")
	bool bIncludeAlgorithmName = false;

	/** Name of the algorithm or policy used to collect the data. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bIncludeAlgorithmName"), Category = "Minari Collection Settings|Metadata")
	FString AlgorithmName;

	/** Description of the dataset. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (InlineEditConditionToggle), Category = "Minari Collection Settings|Metadata")
	bool bIncludeDescription = false;

	/** Description of the dataset. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bIncludeDescription"), Category = "Minari Collection Settings|Metadata")
	FString Description;

	/** Whether to record the info dictionaries in the dataset. If false, only observations, actions, rewards, terminations, and truncations are recorded. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Minari Collection Settings")
	bool bRecordInfos = false;

	/** Directory path where Minari datasets will be stored. If empty, uses the default Minari datasets directory (MINARI_DATASETS_PATH environment variable or ~/.minari/datasets/). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (InlineEditConditionToggle), Category = "Minari Collection Settings")
	bool bUseCustomDataPath = false;

	/** Directory path where Minari datasets will be stored. If empty, uses the default Minari datasets directory (MINARI_DATASETS_PATH environment variable or ~/.minari/datasets/). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bUseCustomDataPath"), Category = "Minari Collection Settings")
	FDirectoryPath DataPath;

	/** Settings for logging during data collection. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Minari Collection Settings")
	FImitationLoggingSettings LoggingSettings;

	/**
	 * @brief Generate command-line arguments for the Minari collection script.
	 * @param[in,out] ArgBuilder The argument builder to populate with settings.
	 */
	virtual void GenerateImitationArgs(FScriptArgBuilder& ArgBuilder) const override;

	/**
	 * @brief Virtual destructor.
	 */
	virtual ~FMinariCollectionSettings();
};
