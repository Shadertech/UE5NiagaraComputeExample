// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#include "Actors/ST_ComputeRPDrawer.h"
#include "GraphBuilder/ST_BoidsGB.h"
#include "PixelShaders/ST_BoidsDrawerPS.h"
#include "Settings/ST_ComputeRPExampleSettings.h"
#include "NiagaraFunctionLibrary.h"
#include "GraphBuilder/ST_BoidsDrawerGB.h"

DEFINE_LOG_CATEGORY(LogComputeRPDrawer);

// ____________________________________________ Constructor

AST_ComputeRPDrawer::AST_ComputeRPDrawer()
{
	Niagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara"));
	Niagara->SetupAttachment(RootComponent);
	Niagara->bAutoActivate = false;

	Plane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Plane"));
	Plane->SetupAttachment(RootComponent);
}

void AST_ComputeRPDrawer::BeginPlay()
{
	Super::BeginPlay();
}

void AST_ComputeRPDrawer::BeginDestroy()
{
	if (Niagara != nullptr )
	{
		Niagara->DeactivateImmediate();
	}

	Super::BeginDestroy();

	if (RenderTarget)
	{
		RenderTarget->ConditionalBeginDestroy(); // Destroy the render target texture
		RenderTarget = nullptr; // Reset the pointer
	}
}

void AST_ComputeRPDrawer::InitComputeShader_GameThread()
{
	check(IsInGameThread());

	CreateRenderTexture();

	const UST_ComputeRPExampleSettings* ComputeRPExampleSettings = UST_ComputeRPExampleSettings::GetComputeRPExampleSettings();
	UMaterialInterface* MatInterface = ComputeRPExampleSettings->BoidsDrawMatInterface.LoadSynchronous();
	if (MatInterface != nullptr && RenderTarget != nullptr)
	{
		MID = UMaterialInstanceDynamic::Create(MatInterface, nullptr);
		Plane->SetMaterial(0, MID);
		MID->SetTextureParameterValue(FName("BoidsPos"), RenderTarget);
	}

	Super::InitComputeShader_GameThread();

	BoundsMatrix = FMatrix::Identity.ConcatTranslation(GetActorLocation());
	BoidsArray.Empty(); // Clear any existing elements in the array

	BoidsPingPongBuffer = FST_PingPongBuffer();
	BoidsRDGStateData = FST_BoidsRDGStateData(3, 3);

	Niagara->SetAsset(ComputeRPExampleSettings->BoidsDrawerVFX.LoadSynchronous());

	if (SetConstantParameters() && SetDynamicParameters())
	{
		Niagara->Activate(true);
	}
}

void AST_ComputeRPDrawer::InitComputeShader_RenderThread(FRHICommandListImmediate& RHICmdList)
{
	check(IsInRenderingThread());

	BoidsRDGStateData.ClearPasses();
	BoidsRDGStateData.InitFence = RHICreateGPUFence(TEXT("BoidsInitFence"));

	FRDGBuilder GraphBuilder(RHICmdList);

	FST_GraphBullder_Boids::InitBoidsExample_RenderThread(GraphBuilder, *GetOwnerName(), BoidsArray, BoidCurrentParameters, BoidsRDGStateData, BoidsPingPongBuffer);
	FST_ComputeShader_BoidsDrawer::InitBoidsDrawerExample_RenderThread(GraphBuilder, BoidsRDGStateData, BoidsArray, BoidsPingPongBuffer);
	FST_PixelShader_BoidsDrawer::InitDrawToRenderTargetExample_RenderThread(GraphBuilder, BoidsRDGStateData, BoidsArray, RenderTarget, RenderTargetTexture);

	GraphBuilder.Execute();

	RHICmdList.WriteGPUFence(BoidsRDGStateData.InitFence);
}

// ____________________________________________ Execute Compute Shader

void AST_ComputeRPDrawer::ExecuteComputeShader_GameThread(float DeltaTime)
{
	check(IsInGameThread());

	Super::ExecuteComputeShader_GameThread(DeltaTime);

	SetDynamicParameters();
}

void AST_ComputeRPDrawer::ExecuteComputeShader_RenderThread(FRHICommandListImmediate& RHICmdList)
{
	check(IsInRenderingThread());

	BoidsRDGStateData.ClearPasses();
	if (BoidsRDGStateData.WaitingOnInitFence())
	{
		return;
	}

	FRDGBuilder GraphBuilder(RHICmdList);

	// The graph will help us figure out when the GPU memory is needed, and only have it allocated from then, so this makes memory management a lot easier and nicer!
	//FRDGTextureDesc ComputeShaderOutputDesc = FRDGTextureDesc::Create2D(FIntPoint(RenderTarget->SizeX, RenderTarget->SizeY), EPixelFormat::PF_R32_UINT, FClearValueBinding::None, ETextureCreateFlags::RenderTargetable | ETextureCreateFlags::UAV | ETextureCreateFlags::ShaderResource);
	FRDGTextureDesc ComputeShaderOutputDesc = FRDGTextureDesc::Create2D(FIntPoint(RenderTarget->SizeX, RenderTarget->SizeY), EPixelFormat::PF_A32B32G32R32F, FClearValueBinding::None, ETextureCreateFlags::RenderTargetable | ETextureCreateFlags::UAV | ETextureCreateFlags::ShaderResource);
	FRDGTextureRef OutputTexture = GraphBuilder.CreateTexture(ComputeShaderOutputDesc, TEXT("ShaderPlugin_ComputeShaderOutput"), ERDGTextureFlags::None);
	FST_GraphBullder_Boids::ExecuteBoidsExample_RenderThread(GraphBuilder, *GetOwnerName(), BoidCurrentParameters, BoidsRDGStateData, BoidsPingPongBuffer, LastDeltaTime);
	FST_ComputeShader_BoidsDrawer::ExecuteBoidsDrawerExample_RenderThread(GraphBuilder, BoidsRDGStateData, BoidCurrentParameters, BoidsPingPongBuffer, GraphBuilder.CreateUAV(OutputTexture), RenderTarget);
	FST_PixelShader_BoidsDrawer::DrawToRenderTargetExample_RenderThread(GraphBuilder, BoidsRDGStateData, GraphBuilder.CreateSRV(OutputTexture), RenderTarget, RenderTargetTexture);

	GraphBuilder.Execute();
}

void AST_ComputeRPDrawer::DisposeComputeShader_GameThread()
{
	check(IsInGameThread());

	Super::DisposeComputeShader_GameThread();

	RenderTargetTexture = nullptr;
}

void AST_ComputeRPDrawer::DisposeComputeShader_RenderThread(FRHICommandListImmediate& RHICmdList)
{
	check(IsInRenderingThread());

	BoidsRDGStateData.Dispose();
}

bool AST_ComputeRPDrawer::SetConstantParameters()
{
	const UST_ComputeRPExampleSettings* ComputeRPExampleSettings = UST_ComputeRPExampleSettings::GetComputeRPExampleSettings();
	BoidCurrentParameters.ConstantParameters = ComputeRPExampleSettings->BoidDrawerConstantParameters;

	// Resize the array to accommodate numboids elements
	BoidsArray.SetNum(BoidCurrentParameters.ConstantParameters.numBoids);

	if (Niagara == nullptr || Niagara->GetAsset() == nullptr)
	{
		return false;
	}

	UNiagaraFunctionLibrary::SetTextureObject(Niagara, "boidPositions", RenderTarget);

	Niagara->SetIntParameter("numBoids", BoidCurrentParameters.ConstantParameters.numBoids);

	return true;
}

bool AST_ComputeRPDrawer::SetDynamicParameters()
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

	Niagara->SetFloatParameter("meshScale", BoidCurrentParameters.DynamicParameters.meshScale);
	Niagara->SetFloatParameter("worldScale", BoidCurrentParameters.worldScale);

	return true;
}

void AST_ComputeRPDrawer::CreateRenderTexture()
{
	if (RenderTarget != nullptr)
	{
		return;
	}

	FName TextureName = FName(TEXT("RT_BoidPositions"));
	int32 SizeX = 32; // Width
	int32 SizeY = 32; // Height

	RenderTarget = NewObject<UTextureRenderTarget2D>(this, TextureName);
	RenderTarget->ClearColor = FLinearColor::Black;
	RenderTarget->TargetGamma = 0.0f;
	RenderTarget->LODGroup = TextureGroup::TEXTUREGROUP_Pixels2D;
	RenderTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA32f;
	RenderTarget->AddressX = TextureAddress::TA_Wrap;
	RenderTarget->AddressY = TextureAddress::TA_Wrap;
	RenderTarget->InitAutoFormat(SizeX, SizeY);
	RenderTarget->UpdateResourceImmediate(true);
}