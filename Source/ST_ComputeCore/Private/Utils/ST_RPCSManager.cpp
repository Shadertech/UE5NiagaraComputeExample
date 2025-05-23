// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#include "Utils/ST_RPCSManager.h"
#include "Kismet/GameplayStatics.h"
#include "SceneRendering.h"
#include "ScenePrivate.h"
#include "Engine/Engine.h"
#include "RHICommandList.h"

DEFINE_LOG_CATEGORY(LogRPCSManager);

DECLARE_GPU_STAT_NAMED(ComputeRPExample_Render, TEXT("ComputeRPExample: Root Render"));
DECLARE_GPU_STAT_NAMED(ComputeRPExample_Compute, TEXT("ComputeRPExample: Render Compute Shader"));
DECLARE_GPU_STAT_NAMED(ComputeRPExample_Pixel, TEXT("ComputeRPExample: Render Pixel Shader"));

// ____________________________________________ Constructor

UST_RPCSManager::UST_RPCSManager()
{
}

// ____________________________________________ BeginPlay

void UST_RPCSManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (GEngine != nullptr)
	{
		GEngine->SetMaxFPS(MaxFPS);
	}
}

// ____________________________________________ BeginDestroy

void UST_RPCSManager::Deinitialize()
{
	Super::Deinitialize();
}

// ____________________________________________ Register

void UST_RPCSManager::Register(IST_ManagedRPCSInterface* ManagedRPCS)
{
	if (!ManagedCSs.Contains(ManagedRPCS))
	{
		ManagedRPCS->InitComputeShader_GameThread();
		UE_LOG(LogTemp, Log, TEXT("RPCSManager registered an interface"));

		ENQUEUE_RENDER_COMMAND(FRPCSRunner)(
			[ManagedRPCS](FRHICommandListImmediate& RHICmdList)
			{
				QUICK_SCOPE_CYCLE_COUNTER(STAT_RPCSManager_Init); // Used to gather CPU profiling data for Unreal Insights! Insights is a really powerful tool for debugging CPU stats, memory and networking.
				SCOPED_DRAW_EVENT(RHICmdList, ComputeRPExample_Init); // Used to profile GPU activity and add metadata to be consumed by for example RenderDoc

				ManagedRPCS->InitComputeShader_RenderThread(RHICmdList);
			});

		// Waits for pending fence commands to retire.
		RenderCommandFence.BeginFence();
		RenderCommandFence.Wait();

		ManagedCSs.Add(ManagedRPCS);
	}
}

void UST_RPCSManager::Deregister(IST_ManagedRPCSInterface* ManagedRPCS)
{
	if (ManagedCSs.Contains(ManagedRPCS))
	{
		ManagedCSs.Remove(ManagedRPCS);

		ENQUEUE_RENDER_COMMAND(FRPCSRunner)(
			[ManagedRPCS](FRHICommandListImmediate& RHICmdList)
			{
				QUICK_SCOPE_CYCLE_COUNTER(STAT_RPCSManager_Dispose); // Used to gather CPU profiling data for Unreal Insights! Insights is a really powerful tool for debugging CPU stats, memory and networking.
				SCOPED_DRAW_EVENT(RHICmdList, ComputeRPExample_Dispose); // Used to profile GPU activity and add metadata to be consumed by for example RenderDoc

				ManagedRPCS->DisposeComputeShader_RenderThread(RHICmdList);
			});

		// Waits for pending fence commands to retire.
		RenderCommandFence.BeginFence();
		RenderCommandFence.Wait();

		ManagedRPCS->DisposeComputeShader_GameThread();
		UE_LOG(LogTemp, Log, TEXT("RPCSManager deregistered an interface"));
	}
}

// ____________________________________________ Tick

void UST_RPCSManager::Tick(float DeltaTime)
{
	if (bEnabled)
	{
		DispatchComputeShaders(DeltaTime);
	}
}

TStatId UST_RPCSManager::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UST_RPCSManager, STATGROUP_Tickables);
}

bool UST_RPCSManager::IsTickable() const
{
	// Return true if your subsystem should tick
	return true;
}

void UST_RPCSManager::DispatchComputeShaders(float DeltaTime)
{
	// Skip this execution round if we are already executing
	if (bIsExecuting)
	{
		UE_LOG(LogRPCSManager, Error, TEXT("Compute Shader Manager: Skip execution"));
		return;
	}

	UST_RPCSManager* RPCSManager = this;

	// ____________________ Execute

	bIsExecuting = true;

	for (IST_ManagedRPCSInterface* ManagedRPCS : ManagedCSs)
	{
		ManagedRPCS->ExecuteComputeShader_GameThread(DeltaTime);
	}

	ENQUEUE_RENDER_COMMAND(FRPCSRunner)(
		[RPCSManager](FRHICommandListImmediate& RHICmdList)
		{
			QUICK_SCOPE_CYCLE_COUNTER(STAT_RPCSManager_Execute); // Used to gather CPU profiling data for Unreal Insights! Insights is a really powerful tool for debugging CPU stats, memory and networking.
			SCOPED_DRAW_EVENT(RHICmdList, RPCSManager_Execute); // Used to profile GPU activity and add metadata to be consumed by for example RenderDoc

			for (IST_ManagedRPCSInterface* ManagedRPCS : RPCSManager->ManagedCSs)
			{
				ManagedRPCS->ExecuteComputeShader_RenderThread(RHICmdList);
			}

			RPCSManager->bIsExecuting = false;

			//RHICmdList.FlushRPCSCache();
		});

	// Waits for pending fence commands to retire.
	RenderCommandFence.BeginFence();
	RenderCommandFence.Wait();
}

// ____________________________________________ Static Get

UST_RPCSManager* UST_RPCSManager::Get(UWorld* World)
{
	if (World == nullptr)
	{
		return nullptr;
	}

	return World->GetSubsystem<UST_RPCSManager>();
}