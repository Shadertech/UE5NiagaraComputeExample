// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#include "SceneView/ST_SceneUBtoMatSceneViewExtension.h"
#include "ComputeShaders/ST_BoidsRPSUBCS.h"
#include "SceneRendererInterface.h"
#include "Data/ST_BoidsLib.h"
#include "Utils/ST_ComputeFunctionLibrary.h"
#include "SystemTextures.h"

FST_SceneUBtoMatSceneViewExtension::FST_SceneUBtoMatSceneViewExtension(const FAutoRegister& AutoRegister)
	: FSceneViewExtensionBase(AutoRegister)
{
}

FST_SceneUBtoMatSceneViewExtension::~FST_SceneUBtoMatSceneViewExtension()
{
}

void FST_SceneUBtoMatSceneViewExtension::PreRenderView_RenderThread(FRDGBuilder& GraphBuilder, FSceneView& InView)
{
}

void FST_SceneUBtoMatSceneViewExtension::SetupViewFamily(FSceneViewFamily& InViewFamily)
{
	BoidsSceneUBtoMatSceneViewExt_RT.numBoids = BoidsSceneUBtoMatSceneViewExt_GT.numBoids;
	BoidsSceneUBtoMatSceneViewExt_RT.ReadPooled = BoidsSceneUBtoMatSceneViewExt_GT.ReadPooled;
}

void FST_SceneUBtoMatSceneViewExtension::PreRenderViewFamily_RenderThread(FRDGBuilder& GraphBuilder, FSceneViewFamily& InViewFamily)
{
	ISceneRenderer* sceneRenderer = InViewFamily.GetSceneRenderer();
	if (sceneRenderer == nullptr)
	{
		return;
	}

	FSceneUniformBuffer& sceneUniformBuffer = sceneRenderer->GetSceneUniforms();
	FST_ExampleSceneUniformBufferParams params;

	if (BoidsSceneUBtoMatSceneViewExt_RT.ReadPooled != nullptr
		&& BoidsSceneUBtoMatSceneViewExt_RT.numBoids > 0)
	{
		FRDGBufferRef readRef = nullptr;
		FRDGBufferSRVRef readSRV = nullptr;
		UST_ComputeFunctionLibrary::RegisterSRV(GraphBuilder, BoidsSceneUBtoMatSceneViewExt_RT.ReadPooled, TEXT("ComputeExample.boidsIn"), readRef, readSRV);
		params.boidsIn = readSRV;
		params.numBoids = BoidsSceneUBtoMatSceneViewExt_RT.numBoids;
	}
	else
	{
		params.numBoids = 0;
		FRDGBufferRef DummyBuffer = GSystemTextures.GetDefaultStructuredBuffer(GraphBuilder, 4);
		params.boidsIn = GraphBuilder.CreateSRV(DummyBuffer);
	}

	sceneUniformBuffer.Set(SceneUB::ComputeExample, params);
}

void FST_SceneUBtoMatSceneViewExtension::SetBoidsData(int32 numBoids, TRefCountPtr<FRDGPooledBuffer> InBuffer)
{
	BoidsSceneUBtoMatSceneViewExt_GT.ReadPooled = InBuffer;
	BoidsSceneUBtoMatSceneViewExt_GT.numBoids = numBoids;
}

void FST_SceneUBtoMatSceneViewExtension::DisposeBoidsBuffer()
{
	BoidsSceneUBtoMatSceneViewExt_GT.ReleaseData();
	BoidsSceneUBtoMatSceneViewExt_RT.ReleaseData();
}