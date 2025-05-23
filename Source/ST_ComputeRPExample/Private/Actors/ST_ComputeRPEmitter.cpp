// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#include "Actors/ST_ComputeRPEmitter.h"
#include "GraphBuilder/ST_BoidsGB.h"
#include "Settings/ST_ComputeRPExampleSettings.h"
#include "Niagara/ST_NDIBoidsExampleBufferFunctionLibrary.h"

DEFINE_LOG_CATEGORY(LogComputeRPEmitter);

// ____________________________________________ Constructor

AST_ComputeRPEmitter::AST_ComputeRPEmitter()
{
	Niagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara"));
	Niagara->SetupAttachment(RootComponent);
	Niagara->bAutoActivate = false;
}

// ____________________________________________ BeginPlay

void AST_ComputeRPEmitter::BeginPlay()
{
	Super::BeginPlay();
}

// ____________________________________________ BeginDestroy

void AST_ComputeRPEmitter::BeginDestroy()
{
	if (Niagara != nullptr)
	{
		Niagara->DeactivateImmediate();
	}
	Super::BeginDestroy();
}

// ____________________________________________ Init Compute Shader

void AST_ComputeRPEmitter::InitComputeShader_GameThread()
{
	check(IsInGameThread());

	Super::InitComputeShader_GameThread();

	BoundsMatrix = FMatrix::Identity.ConcatTranslation(GetActorLocation());
	BoidsArray.Empty(); // Clear any existing elements in the array

	BoidsPingPongBuffer = FST_PingPongBuffer();
	BoidsRDGStateData = FST_BoidsRDGStateData(1, 1);

	Niagara->SetAsset(GetNiagaraSystem().LoadSynchronous());

	if (SetConstantParameters() && SetDynamicParameters())
	{
		Niagara->Activate(true);
	}
}

void AST_ComputeRPEmitter::InitComputeShader_RenderThread(FRHICommandListImmediate& RHICmdList)
{
	check(IsInRenderingThread());

	BoidsRDGStateData.ClearPasses();
	BoidsRDGStateData.InitFence = RHICreateGPUFence(TEXT("BoidsInitFence"));

	FRDGBuilder GraphBuilder(RHICmdList);

	FST_GraphBullder_Boids::InitBoidsExample_RenderThread(GraphBuilder, *GetOwnerName(), BoidsArray, BoidCurrentParameters, BoidsRDGStateData, BoidsPingPongBuffer);

	GraphBuilder.Execute();

	RHICmdList.WriteGPUFence(BoidsRDGStateData.InitFence);
}

// ____________________________________________ Execute Compute Shader

void AST_ComputeRPEmitter::ExecuteComputeShader_GameThread(float DeltaTime)
{
	check(IsInGameThread());

	Super::ExecuteComputeShader_GameThread(DeltaTime);

	SetDynamicParameters();
}

void AST_ComputeRPEmitter::ExecuteComputeShader_RenderThread(FRHICommandListImmediate& RHICmdList)
{
	check(IsInRenderingThread());

	BoidsRDGStateData.ClearPasses();
	if (BoidsRDGStateData.WaitingOnInitFence())
	{
		return;
	}

	FRDGBuilder GraphBuilder(RHICmdList);

	FST_GraphBullder_Boids::ExecuteBoidsExample_RenderThread(GraphBuilder, *GetOwnerName(), BoidCurrentParameters, BoidsRDGStateData, BoidsPingPongBuffer, LastDeltaTime);

	GraphBuilder.Execute();
}

// ____________________________________________ DisposeComputeShader
void AST_ComputeRPEmitter::DisposeComputeShader_GameThread()
{
	check(IsInGameThread());
	Super::DisposeComputeShader_GameThread();
}

void AST_ComputeRPEmitter::DisposeComputeShader_RenderThread(FRHICommandListImmediate& RHICmdList)
{
	check(IsInRenderingThread());

	BoidsRDGStateData.Dispose();
}

TSoftObjectPtr<UNiagaraSystem> AST_ComputeRPEmitter::GetNiagaraSystem() const
{
	const UST_ComputeRPExampleSettings* ComputeRPExampleSettings = UST_ComputeRPExampleSettings::GetComputeRPExampleSettings();
	return ComputeRPExampleSettings->BoidsEmitterVFX;
}

bool AST_ComputeRPEmitter::SetConstantParameters()
{
	const UST_ComputeRPExampleSettings* ComputeRPExampleSettings = UST_ComputeRPExampleSettings::GetComputeRPExampleSettings();
	BoidCurrentParameters.ConstantParameters = ComputeRPExampleSettings->BoidEmitterConstantParameters;

	// Resize the array to accommodate numboids elements
	BoidsArray.SetNum(BoidCurrentParameters.ConstantParameters.numBoids);

	if (Niagara == nullptr || Niagara->GetAsset() == nullptr)
	{
		return false;
	}

	UST_NDIBoidsExampleBufferFunctionLibrary::SetNiagaraBoidsExampleBuffer(Niagara, "boidsIn", BoidCurrentParameters.ConstantParameters.numBoids, nullptr);

	Niagara->SetIntParameter("numBoids", BoidCurrentParameters.ConstantParameters.numBoids);

	return true;
}

bool AST_ComputeRPEmitter::SetDynamicParameters()
{
	const UST_ComputeRPExampleSettings* ComputeRPExampleSettings = UST_ComputeRPExampleSettings::GetComputeRPExampleSettings();
	BoidCurrentParameters.DynamicParameters = ComputeRPExampleSettings->BoidDynamicParameters;

	BoidCurrentParameters.boundsRadius = BoundsConstantParameters.Radius;
	BoidCurrentParameters.transformMatrix = (FMatrix44f)BoundsMatrix;
	BoidCurrentParameters.inverseTransformMatrix = (FMatrix44f)BoundsMatrix.Inverse();

	if (Niagara == nullptr || Niagara->GetAsset() == nullptr)
	{
		return false;
	}

	UST_NDIBoidsExampleBufferFunctionLibrary::SetNiagaraBoidsExampleBuffer(Niagara, "boidsIn", BoidCurrentParameters.ConstantParameters.numBoids, BoidsPingPongBuffer.ReadPooled);

	Niagara->SetFloatParameter("meshScale", BoidCurrentParameters.DynamicParameters.meshScale);
	Niagara->SetFloatParameter("worldScale", BoidCurrentParameters.worldScale);

	return true;
}