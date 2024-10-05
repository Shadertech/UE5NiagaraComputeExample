// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/ComputeRPSUBExampleActor.h"
#include "SceneView/SceneUBtoMatManager.h"

#include "Materials/MaterialInstanceDynamic.h"
#include "Settings/ComputeExampleSettings.h"
#include "GraphBuilder/FlattenBoidsGB.h"
#include "GraphBuilder/BoidsGB.h"

DEFINE_LOG_CATEGORY(LogComputeRPSUBExampleActor);

// Sets default values
AComputeRPSUBExampleActor::AComputeRPSUBExampleActor()
{
}

void AComputeRPSUBExampleActor::InitComputeShader_GameThread()
{
	const UComputeExampleSettings* computeExampleSettings = UComputeExampleSettings::GetComputeExampleSettings();
	UMaterialInterface* MatInterface = computeExampleSettings->BoidsMatInterface.LoadSynchronous();
	MatiD = UMaterialInstanceDynamic::Create(MatInterface, this);

	Super::InitComputeShader_GameThread();
	BoidsRDGStateData = FBoidsRDGStateData(2, 2);

	USceneUBtoMatManager* UBtoMatManager = GEngine->GetEngineSubsystem<USceneUBtoMatManager>();
	if (UBtoMatManager)
	{
		ViewExtension = UBtoMatManager->GetSceneViewExtension();
	}
}

void AComputeRPSUBExampleActor::InitComputeShader_RenderThread(FRHICommandListImmediate& RHICmdList)
{
	check(IsInRenderingThread());

	BoidsRDGStateData.ClearPasses();
	BoidsRDGStateData.InitFence = RHICreateGPUFence(TEXT("BoidsInitFence"));

	FRDGBuilder GraphBuilder(RHICmdList);

	FGraphBullder_Boids::InitBoidsExample_RenderThread(GraphBuilder, *GetOwnerName(), BoidsArray, BoidCurrentParameters, BoidsRDGStateData, BoidsPingPongBuffer);
	FGraphBullder_FlattenBoids::InitFlattenBoidsExample_RenderThread(GraphBuilder, *GetOwnerName(), BoidsArray, BoidsRDGStateData, FlattenBoidsPingPongBuffer, BoidsPingPongBuffer);

	GraphBuilder.Execute();

	RHICmdList.WriteGPUFence(BoidsRDGStateData.InitFence);
}

void AComputeRPSUBExampleActor::ExecuteComputeShader_GameThread(float DeltaTime)
{
	Super::ExecuteComputeShader_GameThread(DeltaTime);
}

void AComputeRPSUBExampleActor::ExecuteComputeShader_RenderThread(FRHICommandListImmediate& RHICmdList)
{
	check(IsInRenderingThread());

	BoidsRDGStateData.ClearPasses();
	if (BoidsRDGStateData.WaitingOnInitFence())
	{
		return;
	}

	FRDGBuilder GraphBuilder(RHICmdList);

	FGraphBullder_Boids::ExecuteBoidsExample_RenderThread(GraphBuilder, *GetOwnerName(), BoidCurrentParameters, BoidsRDGStateData, BoidsPingPongBuffer, LastDeltaTime);
	FGraphBullder_FlattenBoids::ExecuteFlattenBoidsExample_RenderThread(GraphBuilder, *GetOwnerName(), BoidCurrentParameters, BoidsRDGStateData, FlattenBoidsPingPongBuffer, BoidsPingPongBuffer, LastDeltaTime);

	GraphBuilder.Execute();
}


void AComputeRPSUBExampleActor::DisposeComputeShader_GameThread()
{
	Super::DisposeComputeShader_GameThread();

	if (ViewExtension.IsValid())
	{
		ViewExtension->DisposeBoidsBuffer();
	}
}

void AComputeRPSUBExampleActor::DisposeComputeShader_RenderThread(FRHICommandListImmediate& RHICmdList)
{
	check(IsInRenderingThread());

	Super::DisposeComputeShader_RenderThread(RHICmdList);
}

bool AComputeRPSUBExampleActor::SetConstantParameters()
{
	bool bSuccess = Super::SetConstantParameters();

	if (bSuccess && MatiD != nullptr)
	{
		Niagara->SetVariableMaterial("boidsMat", MatiD);
	}

	return bSuccess;
}

TSoftObjectPtr<UNiagaraSystem> AComputeRPSUBExampleActor::GetNiagaraSystem() const
{
	const UComputeExampleSettings* computeExampleSettings = UComputeExampleSettings::GetComputeExampleSettings();
	return computeExampleSettings->BoidsEmitterMaterialVFX;
}

bool AComputeRPSUBExampleActor::SetDynamicParameters()
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