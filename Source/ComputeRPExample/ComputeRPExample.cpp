// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#include "ComputeRPExample.h"
#include "ComputeCore.h"
#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FComputeRPExampleModule"

void FComputeRPExampleModule::StartupModule()
{
}

void FComputeRPExampleModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FComputeRPExampleModule, ComputeRPExample)