// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#include "Actors/ST_ComputeRPSUBExampleActor.h"
#include "SceneView/ST_SceneUBtoMatManager.h"

#include "Materials/MaterialInstanceDynamic.h"
#include "Settings/ST_ComputeRPSUBExampleSettings.h"
#include "GraphBuilder/ST_FlattenBoidsGB.h"
#include "GraphBuilder/ST_BoidsGB.h"

DEFINE_LOG_CATEGORY(LogComputeRPSUBExampleActor);

// Sets default values
AST_ComputeRPSUBExampleActor::AST_ComputeRPSUBExampleActor()
{
}

void AST_ComputeRPSUBExampleActor::InitComputeShader_GameThread()
{
	const UST_ComputeRPSUBExampleSettings* ComputeRPSUBExampleSettings = UST_ComputeRPSUBExampleSettings::GetComputeRPSUBExampleSettings();
	UMaterialInterface* MatInterface = ComputeRPSUBExampleSettings->BoidsMatInterface.LoadSynchronous();
	MatiD = UMaterialInstanceDynamic::Create(MatInterface, this);

	Super::InitComputeShader_GameThread();
	BoidsRDGStateData = FST_BoidsRDGStateData(2, 2);

	UST_SceneUBtoMatManager* UBtoMatManager = GEngine->GetEngineSubsystem<UST_SceneUBtoMatManager>();
	if (UBtoMatManager)
	{
		ViewExtension = UBtoMatManager->GetSceneViewExtension();
	}
}

void AST_ComputeRPSUBExampleActor::InitComputeShader_RenderThread(FRHICommandListImmediate& RHICmdList)
{
	check(IsInRenderingThread());

	BoidsRDGStateData.ClearPasses();
	BoidsRDGStateData.InitFence = RHICreateGPUFence(TEXT("BoidsInitFence"));

	FRDGBuilder GraphBuilder(RHICmdList);

	FST_GraphBullder_Boids::InitBoidsExample_RenderThread(GraphBuilder, *GetOwnerName(), BoidsArray, BoidCurrentParameters, BoidsRDGStateData, BoidsPingPongBuffer);
	FST_GraphBullder_FlattenBoids::InitFlattenBoidsExample_RenderThread(GraphBuilder, *GetOwnerName(), BoidsArray, BoidsRDGStateData, FlattenBoidsPingPongBuffer, BoidsPingPongBuffer);

	GraphBuilder.Execute();

	RHICmdList.WriteGPUFence(BoidsRDGStateData.InitFence);
}

void AST_ComputeRPSUBExampleActor::ExecuteComputeShader_GameThread(float DeltaTime)
{
	Super::ExecuteComputeShader_GameThread(DeltaTime);
}

void AST_ComputeRPSUBExampleActor::ExecuteComputeShader_RenderThread(FRHICommandListImmediate& RHICmdList)
{
	check(IsInRenderingThread());

	BoidsRDGStateData.ClearPasses();
	if (BoidsRDGStateData.WaitingOnInitFence())
	{
		return;
	}

	FRDGBuilder GraphBuilder(RHICmdList);

	FST_GraphBullder_Boids::ExecuteBoidsExample_RenderThread(GraphBuilder, *GetOwnerName(), BoidCurrentParameters, BoidsRDGStateData, BoidsPingPongBuffer, LastDeltaTime);
	FST_GraphBullder_FlattenBoids::ExecuteFlattenBoidsExample_RenderThread(GraphBuilder, *GetOwnerName(), BoidCurrentParameters, BoidsRDGStateData, FlattenBoidsPingPongBuffer, BoidsPingPongBuffer, LastDeltaTime);

	GraphBuilder.Execute();
}


void AST_ComputeRPSUBExampleActor::DisposeComputeShader_GameThread()
{
	Super::DisposeComputeShader_GameThread();

	if (ViewExtension.IsValid())
	{
		ViewExtension->DisposeBoidsBuffer();
	}
}

void AST_ComputeRPSUBExampleActor::DisposeComputeShader_RenderThread(FRHICommandListImmediate& RHICmdList)
{
	check(IsInRenderingThread());

	Super::DisposeComputeShader_RenderThread(RHICmdList);
}

bool AST_ComputeRPSUBExampleActor::SetConstantParameters()
{
	bool bSuccess = Super::SetConstantParameters();

	if (bSuccess && MatiD != nullptr)
	{
		Niagara->SetVariableMaterial("boidsMat", MatiD);
	}

	return bSuccess;
}

TSoftObjectPtr<UNiagaraSystem> AST_ComputeRPSUBExampleActor::GetNiagaraSystem() const
{
	const UST_ComputeRPSUBExampleSettings* ComputeRPSUBExampleSettings = UST_ComputeRPSUBExampleSettings::GetComputeRPSUBExampleSettings();
	return ComputeRPSUBExampleSettings->BoidsEmitterMaterialVFX;
}

bool AST_ComputeRPSUBExampleActor::SetDynamicParameters()
{
	bool bSuccess = Super::SetDynamicParameters();

	if (bSuccess)
	{
		if (ViewExtension.IsValid())
		{
			ViewExtension->SetBoidsData(BoidCurrentParameters.ConstantParameters.numBoids, FlattenBoidsPingPongBuffer.ReadPooled);
		}
	}

	return bSuccess;
}