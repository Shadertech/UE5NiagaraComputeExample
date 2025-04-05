// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#include "ComputeRPLegacyExample.h"
#include "ComputeCore.h"

#define LOCTEXT_NAMESPACE "FComputeRPLegacyExampleModule"

void FComputeRPLegacyExampleModule::StartupModule()
{
}

void FComputeRPLegacyExampleModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FComputeRPLegacyExampleModule, ComputeRPLegacyExample)