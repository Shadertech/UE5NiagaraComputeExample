// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#include "ST_NiagaraExample.h"
#include "ST_ComputeCore.h"

#define LOCTEXT_NAMESPACE "FST_NiagaraExampleModule"

void FST_NiagaraExampleModule::StartupModule()
{
}

void FST_NiagaraExampleModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FST_NiagaraExampleModule, ST_NiagaraExample)