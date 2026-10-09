// Copyright (c) 2025 Advanced Micro Devices, Inc. All Rights Reserved.

#include "ImitationSettings/Minari/MinariCollectionSettings.h"

void FMinariCollectionSettings::GenerateImitationArgs(FScriptArgBuilder& ArgBuilder) const
{
	// Add positional arguments for running minari collect script
	ArgBuilder.AddPositionalArgument("minari");
	ArgBuilder.AddPositionalArgument("collect");
	// Must match ``schola minari collect <simulator>`` (connect to UE already running, same as gym RL training from editor).
	ArgBuilder.AddPositionalArgument(TEXT("external"));

	// Add dataset ID argument
	ArgBuilder.AddStringArg(TEXT("dataset-id"), DatasetId);

	// Add collection parameters
	ArgBuilder.AddIntArg(TEXT("num-steps"), NumSteps);
	
	if (Seed >= 0)
	{
		ArgBuilder.AddIntArg(TEXT("seed"), Seed);
	}

	// Add metadata arguments if enabled
	ArgBuilder.AddConditionalStringArg(TEXT("author"), Author, bIncludeAuthor);
	ArgBuilder.AddConditionalStringArg(TEXT("author-email"), AuthorEmail, bIncludeAuthorEmail);
	ArgBuilder.AddConditionalStringArg(TEXT("code-permalink"), CodePermalink, bIncludeCodePermalink);
	ArgBuilder.AddConditionalStringArg(TEXT("algorithm-name"), AlgorithmName, bIncludeAlgorithmName);
	ArgBuilder.AddConditionalStringArg(TEXT("description"), Description, bIncludeDescription);

	// Add record_infos flag
	ArgBuilder.AddFlag(TEXT("record-infos"), bRecordInfos);
	
	// Add data path if enabled
	ArgBuilder.AddConditionalStringArg(TEXT("data-path"), DataPath.Path, bUseCustomDataPath);

	// Add logging settings
	LoggingSettings.GenerateImitationArgs(ArgBuilder);
}

FMinariCollectionSettings::~FMinariCollectionSettings()
{
}
