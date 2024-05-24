// Copyright Epic Games, Inc. All Rights Reserved.

#include "ComputeGBExample.h"
#include "ShaderCore.h"
#include "Engine/Engine.h"

#define LOCTEXT_NAMESPACE "FComputeGBExampleModule"

// Declare some GPU stats so we can track them later
DECLARE_GPU_STAT_NAMED(ComputeGBExample_Render, TEXT("ComputeGBExample: Root Render"));
DECLARE_GPU_STAT_NAMED(ComputeGBExample_Compute, TEXT("ComputeGBExample: Render Compute Shader"));
DECLARE_GPU_STAT_NAMED(ComputeGBExample_Pixel, TEXT("ComputeGBExample: Render Pixel Shader"));

void FComputeGBExampleModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FComputeGBExampleModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	if (!GEngine)
	{
		return;
	}

	GEngine->GetPreRenderDelegateEx().RemoveAll(this);
}

void FComputeGBExampleModule::BeginRendering()
{
	if (!GEngine)
	{
		return;
	}

	if (!GEngine->GetPreRenderDelegateEx().IsBoundToObject(this))
	{
		GEngine->GetPreRenderDelegateEx().AddRaw(this, &FComputeGBExampleModule::HandlePreRender_RenderThread);
	}
}

void FComputeGBExampleModule::EndRendering()
{
	if (!GEngine)
	{
		return;
	}

	GEngine->GetPreRenderDelegateEx().RemoveAll(this);
}

void FComputeGBExampleModule::Register(IManagedGBCSInterface* ManagedRPCS)
{
	if (!ManagedCSs.Contains(ManagedRPCS))
	{
		ManagedCSs.Add(ManagedRPCS);
		ManagedRPCS->InitComputeShader_GameThread();
	}

	if (ManagedCSs.Num() > 0)
	{
		BeginRendering();
	}
}

void FComputeGBExampleModule::Deregister(IManagedGBCSInterface* ManagedRPCS)
{
	if (ManagedCSs.Contains(ManagedRPCS))
	{
		ManagedCSs.Remove(ManagedRPCS);
		ManagedRPCS->DisposeComputeShader_GameThread();
	}

	if (ManagedCSs.Num() == 0)
	{
		EndRendering();
	}
}

void FComputeGBExampleModule::HandlePreRender_RenderThread(FRDGBuilder& GraphBuilder)
{
	check(IsInRenderingThread());

	QUICK_SCOPE_CYCLE_COUNTER(ComputeGBExample_Render); // Used to gather CPU profiling data for Unreal Insights! Insights is a really powerful tool for debugging CPU stats, memory and networking.
	SCOPED_DRAW_EVENT(GraphBuilder.RHICmdList, ComputeGBExample_Render); // Used to profile GPU activity and add metadata to be consumed by for example RenderDoc

	for (IManagedGBCSInterface* ManagedCS : ManagedCSs)
	{
		ManagedCS->ExecuteComputeShader_RenderThread(GraphBuilder);
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FComputeGBExampleModule, ComputeGBExample)