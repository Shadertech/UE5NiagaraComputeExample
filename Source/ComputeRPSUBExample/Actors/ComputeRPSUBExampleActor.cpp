// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/ComputeRPSUBExampleActor.h"
#include "SceneView/SceneUBtoMatManager.h"

#include "Materials/MaterialInstanceDynamic.h"
#include "Settings/ComputeExampleSettings.h"
#include "GraphBuilder/FlattenBoidsGB.h"

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

	USceneUBtoMatManager* UBtoMatManager = GEngine->GetEngineSubsystem<USceneUBtoMatManager>();
	if (UBtoMatManager)
	{
		ViewExtension = UBtoMatManager->GetSceneViewExtension();
	}
}

void AComputeRPSUBExampleActor::InitComputeShader_RenderThread(FRHICommandListImmediate& RHICmdList)
{
	check(IsInRenderingThread());
	Super::InitComputeShader_RenderThread(RHICmdList);

	FRDGBuilder GraphBuilder(RHICmdList);

	FlattenBoidsRenderGraphPasses.ClearPasses();

	FGraphBullder_FlattenBoids::InitFlattenBoidsExample_RenderThread(GraphBuilder, *GetOwnerName(), BoidsArray, FlattenBoidsRenderGraphPasses, FlattenBoidsPingPongBuffer, BoidsPingPongBuffer);

	GraphBuilder.Execute();
}

void AComputeRPSUBExampleActor::ExecuteComputeShader_GameThread(float DeltaTime)
{
	Super::ExecuteComputeShader_GameThread(DeltaTime);
}

void AComputeRPSUBExampleActor::ExecuteComputeShader_RenderThread(FRHICommandListImmediate& RHICmdList)
{
	check(IsInRenderingThread());
	Super::ExecuteComputeShader_RenderThread(RHICmdList);

	FRDGBuilder GraphBuilder(RHICmdList);

	FlattenBoidsRenderGraphPasses.ClearPasses();

	FGraphBullder_FlattenBoids::ExecuteFlattenBoidsExample_RenderThread(GraphBuilder, *GetOwnerName(), BoidCurrentParameters, FlattenBoidsRenderGraphPasses, FlattenBoidsPingPongBuffer, BoidsPingPongBuffer, LastDeltaTime);

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

	FlattenBoidsPingPongBuffer.Dispose();
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