// Copyright Epic Games, Inc. All Rights Reserved.

#include "NiagaraExample.h"
#include "ComputeCore.h"

#define LOCTEXT_NAMESPACE "FNiagaraExampleModule"

void FNiagaraExampleModule::StartupModule()
{
}

void FNiagaraExampleModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FNiagaraExampleModule, NiagaraExample)