// Copyright (c) 2024 Advanced Micro Devices, Inc. All Rights Reserved.

#include "Schola.h"
#include "Developer/Settings/Public/ISettingsModule.h"
//#include "TrainingSettings/SubsystemSettings.h"


#define LOCTEXT_NAMESPACE "FScholaModule"

void FScholaModule::StartupModule()
{
	// register settings
	/** 
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");

	if (SettingsModule != nullptr)
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "Schola",
			LOCTEXT("ScholaSettingsName", "Schola"),
			LOCTEXT("ScholaSettingsDescription", "Project settings for Schola plugin"),
			GetMutableDefault<UScholaManagerSubsystemSettings>());
	}
	*/
}

void FScholaModule::ShutdownModule()
{
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");

	if (SettingsModule != nullptr)
	{
		//SettingsModule->UnregisterSettings("Project", "Plugins", "Schola");
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FScholaModule, Schola)