// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#pragma once
#include "SceneViewExtension.h"
#include "RenderGraphBuilder.h"
#include "GlobalShader.h"
#include "RenderGraphResources.h"
#include "RenderGraphResources.h"

struct FST_BoidsSceneUBtoMatSceneViewExt_GameThread
{
	TRefCountPtr<FRDGPooledBuffer> ReadPooled = nullptr;
	int32 numBoids;

	~FST_BoidsSceneUBtoMatSceneViewExt_GameThread()
	{
		ReleaseData();
	}

	void ReleaseData()
	{
		numBoids = 0;
		ReadPooled.SafeRelease();
	}
};

struct FST_BoidsSceneUBtoMatSceneViewExt_RenderThread
{
	TRefCountPtr<FRDGPooledBuffer> ReadPooled = nullptr;
	int32 numBoids;

	~FST_BoidsSceneUBtoMatSceneViewExt_RenderThread()
	{
		ReleaseData();
	}

	void ReleaseData()
	{
		numBoids = 0;
		ReadPooled.SafeRelease();
	}
};

class FST_SceneUBtoMatSceneViewExtension : public FSceneViewExtensionBase
{
public:
	FST_SceneUBtoMatSceneViewExtension(const FAutoRegister& AutoRegister);
	virtual ~FST_SceneUBtoMatSceneViewExtension();
	virtual void PreRenderView_RenderThread(FRDGBuilder& GraphBuilder, FSceneView& InView) override;
	virtual void SetupViewFamily(FSceneViewFamily& InViewFamily) override;
	virtual void SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView) override {}
	virtual void PreRenderViewFamily_RenderThread(FRDGBuilder& RHICmdList, FSceneViewFamily& InViewFamily) override;
	virtual void BeginRenderViewFamily(FSceneViewFamily& InViewFamily) override {}
	virtual bool IsActiveThisFrame_Internal(const FSceneViewExtensionContext& Context) const override { return true; }

	void SetBoidsData(int32 numBoids, TRefCountPtr<FRDGPooledBuffer> InBuffer);
	void DisposeBoidsBuffer();

private:
	FST_BoidsSceneUBtoMatSceneViewExt_GameThread BoidsSceneUBtoMatSceneViewExt_GT;
	FST_BoidsSceneUBtoMatSceneViewExt_RenderThread BoidsSceneUBtoMatSceneViewExt_RT;
};