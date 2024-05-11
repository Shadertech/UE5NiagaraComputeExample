// Copyright Epic Games, Inc. All Rights Reserved.

#include "ComputeRPExample.h"
#include "ShaderCore.h"

#define LOCTEXT_NAMESPACE "FComputeRPExampleModule"

void FComputeRPExampleModule::StartupModule()
{
}

void FComputeRPExampleModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FComputeRPExampleModule, ComputeRPExample)