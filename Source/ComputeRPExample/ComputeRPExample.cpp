// Copyright Epic Games, Inc. All Rights Reserved.

#include "ComputeRPExample.h"
#include "ComputeCore.h"
#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FComputeRPExampleModule"

void FComputeRPExampleModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("ComputeExample"))->GetBaseDir(), TEXT("Shaders/ComputeRPExample"));
	AddShaderSourceDirectoryMapping(TEXT("/ComputeRPExample"), PluginShaderDir);
}

void FComputeRPExampleModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	ResetAllShaderSourceDirectoryMappings();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FComputeRPExampleModule, ComputeRPExample)