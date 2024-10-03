#include "SceneUBtoMatSceneViewExtension.h"
#include "ComputeShaders/BoidsRPSUBCS.h"
#include "SceneRendererInterface.h"
#include "RenderGraphResources.h"
#include "RenderGraphBuilder.h"
#include "Data/BoidsLib.h"
#include "Utils/ComputeFunctionLibrary.h"

FSceneUBtoMatSceneViewExtension::FSceneUBtoMatSceneViewExtension(const FAutoRegister& AutoRegister)
	: FSceneViewExtensionBase(AutoRegister)
{
}

FSceneUBtoMatSceneViewExtension::~FSceneUBtoMatSceneViewExtension()
{
}

void FSceneUBtoMatSceneViewExtension::PreRenderView_RenderThread(FRDGBuilder& GraphBuilder, FSceneView& InView)
{
}

void FSceneUBtoMatSceneViewExtension::SetupViewFamily(FSceneViewFamily& InViewFamily)
{
	BoidsSceneUBtoMatSceneViewExt_RT.numBoids = BoidsSceneUBtoMatSceneViewExt_GT.numBoids;
	BoidsSceneUBtoMatSceneViewExt_RT.ReadPooled = BoidsSceneUBtoMatSceneViewExt_GT.ReadPooled;
}

void FSceneUBtoMatSceneViewExtension::PreRenderViewFamily_RenderThread(FRDGBuilder& GraphBuilder, FSceneViewFamily& InViewFamily)
{
	ISceneRenderer* sceneRenderer = InViewFamily.GetSceneRenderer();
	if (sceneRenderer == nullptr)
	{
		return;
	}

	FSceneUniformBuffer& sceneUniformBuffer = sceneRenderer->GetSceneUniforms();
	FExampleSceneUniformBufferParams params;

	if (BoidsSceneUBtoMatSceneViewExt_RT.ReadPooled != nullptr
		&& BoidsSceneUBtoMatSceneViewExt_RT.numBoids > 0)
	{
		FRDGBufferRef readRef = nullptr;
		FRDGBufferSRVRef readSRV = nullptr;
		UComputeFunctionLibrary::RegisterSRV(GraphBuilder, BoidsSceneUBtoMatSceneViewExt_RT.ReadPooled, TEXT("ComputeExample.boidsIn"), readRef, readSRV);
		params.boidsIn = readSRV;
		params.numBoids = BoidsSceneUBtoMatSceneViewExt_RT.numBoids;
	}
	else
	{
		params.numBoids = 0;
		FRDGBufferRef DummyBuffer = UComputeFunctionLibrary::CreateStructuredBuffer(GraphBuilder, TEXT("ComputeExample.boidsIn"), 4, 1);
		params.boidsIn = GraphBuilder.CreateSRV(DummyBuffer);
	}

	sceneUniformBuffer.Set(SceneUB::ComputeExample, params);
}

void FSceneUBtoMatSceneViewExtension::SetBoidsData(int32 numBoids, TRefCountPtr<FRDGPooledBuffer> InBuffer)
{
	BoidsSceneUBtoMatSceneViewExt_GT.ReadPooled = InBuffer;
	BoidsSceneUBtoMatSceneViewExt_GT.numBoids = numBoids;
}

void FSceneUBtoMatSceneViewExtension::DisposeBoidsBuffer()
{
	BoidsSceneUBtoMatSceneViewExt_GT.ReadPooled = nullptr;
}