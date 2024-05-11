#include "RPCSManager.h"
#include "ComputeRPExample.h"
#include "Kismet/GameplayStatics.h"
#include "SceneRendering.h"
#include "ScenePrivate.h"
#include "Engine/Engine.h"
#include "RHICommandList.h"

DEFINE_LOG_CATEGORY(LogRPCSManager);

// ____________________________________________ Constructor

ARPCSManager::ARPCSManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

// ____________________________________________ BeginPlay

void ARPCSManager::BeginPlay()
{
	Super::BeginPlay();

	if (GEngine != nullptr)
	{
		GEngine->SetMaxFPS(MaxFPS);
	}
}

// ____________________________________________ BeginDestroy

void ARPCSManager::BeginDestroy()
{
	Super::BeginDestroy();
}

// ____________________________________________ Register

void ARPCSManager::Register(IManagedRPCSInterface* ManagedRPCS)
{
	if (!ManagedCSs.Contains(ManagedRPCS))
	{
		ManagedCSs.Add(ManagedRPCS);
	}
}

void ARPCSManager::Deregister(IManagedRPCSInterface* ManagedRPCS)
{
	if (ManagedCSs.Contains(ManagedRPCS))
	{
		ManagedCSs.Remove(ManagedRPCS);
	}
}

// ____________________________________________ Tick

void ARPCSManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bEnabled)
	{
		DispatchComputeShaders(DeltaTime);
	}
}

void ARPCSManager::DispatchComputeShaders(float DeltaTime)
{
	// Skip this execution round if we are already executing
	if (bIsExecuting)
	{
		UE_LOG(LogRPCSManager, Error, TEXT("Compute Shader Manager: Skip execution"));
		return;
	}

	ARPCSManager* RPCSManager = this;

	if (!bIsInit)
	{
		if (--FrameDelay > 0)
		{
			return;
		}

		// ____________________ Init

		bIsInit = true;
		bIsExecuting = true;

		// Init Mesh Distance Fields
		FScene* Scene = GetWorld()->Scene->GetRenderScene();

		// Init Compute Shaders
		for (IManagedRPCSInterface* ManagedRPCS : ManagedCSs)
		{
			ManagedRPCS->InitComputeShader();
		}

		ENQUEUE_RENDER_COMMAND(FRPCSRunner)(
			[RPCSManager](FRHICommandListImmediate& RHICmdList)
		{
			for (IManagedRPCSInterface* ManagedRPCS : RPCSManager->ManagedCSs)
			{
				ManagedRPCS->InitComputeShader_RenderThread(RHICmdList);
			}

			RPCSManager->bIsExecuting = false;
		});

		// Waits for pending fence commands to retire.
		RenderCommandFence.BeginFence();
		RenderCommandFence.Wait();
	}
	else
	{
		// ____________________ Execute

		bIsExecuting = true;

		for (IManagedRPCSInterface* ManagedRPCS : ManagedCSs)
		{
			ManagedRPCS->ExecuteComputeShader(DeltaTime);
		}

		ENQUEUE_RENDER_COMMAND(FRPCSRunner)(
			[RPCSManager](FRHICommandListImmediate& RHICmdList)
		{
			for (IManagedRPCSInterface* ManagedRPCS : RPCSManager->ManagedCSs)
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
}

// ____________________________________________ Static Get

ARPCSManager* ARPCSManager::Get(UWorld* World)
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(World, ARPCSManager::StaticClass(), Actors);

	if (Actors.Num() == 0)
	{
		UE_LOG(LogRPCSManager, Error, TEXT("RPCSManager not found, please add one to the Persistent Level."));
	}
	else if (Actors.Num() > 1)
	{
		UE_LOG(LogRPCSManager, Error, TEXT("Only one RPCSManager is supported, using the first one found."));
	}

	return (Actors.Num() > 0) ? Cast<ARPCSManager>(Actors[0]) : nullptr;
}