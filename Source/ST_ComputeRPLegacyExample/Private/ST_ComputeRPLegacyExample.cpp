// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#include "ST_ComputeRPLegacyExample.h"
#include "ST_ComputeCore.h"

#define LOCTEXT_NAMESPACE "FST_ComputeRPLegacyExampleModule"

void FST_ComputeRPLegacyExampleModule::StartupModule()
{
}

void FST_ComputeRPLegacyExampleModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FST_ComputeRPLegacyExampleModule, ST_ComputeRPLegacyExample)