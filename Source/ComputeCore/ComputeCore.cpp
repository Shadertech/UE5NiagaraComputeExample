// Copyright Epic Games, Inc. All Rights Reserved.

#include "ComputeCore.h"
#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FComputeCoreModule"

void FComputeCoreModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("ComputeExample"))->GetBaseDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/ComputeCore"), PluginShaderDir);
}

void FComputeCoreModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	ResetAllShaderSourceDirectoryMappings();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FComputeCoreModule, ComputeCore)