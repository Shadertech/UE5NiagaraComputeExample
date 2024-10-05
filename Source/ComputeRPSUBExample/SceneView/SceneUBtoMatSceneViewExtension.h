#pragma once
#include "SceneViewExtension.h"
#include "RenderGraphResources.h"

struct FBoidsSceneUBtoMatSceneViewExt_GameThread
{
	TRefCountPtr<FRDGPooledBuffer> ReadPooled = nullptr;
	int32 numBoids;

	~FBoidsSceneUBtoMatSceneViewExt_GameThread()
	{
		ReleaseData();
	}

	void ReleaseData()
	{
		numBoids = 0;
		ReadPooled.SafeRelease();
	}
};

struct FBoidsSceneUBtoMatSceneViewExt_RenderThread
{
	TRefCountPtr<FRDGPooledBuffer> ReadPooled = nullptr;
	int32 numBoids;

	~FBoidsSceneUBtoMatSceneViewExt_RenderThread()
	{
		ReleaseData();
	}

	void ReleaseData()
	{
		numBoids = 0;
		ReadPooled.SafeRelease();
	}
};

class FSceneUBtoMatSceneViewExtension : public FSceneViewExtensionBase
{
public:
	FSceneUBtoMatSceneViewExtension(const FAutoRegister& AutoRegister);
	virtual ~FSceneUBtoMatSceneViewExtension();
	virtual void PreRenderView_RenderThread(FRDGBuilder& GraphBuilder, FSceneView& InView) override;
	virtual void SetupViewFamily(FSceneViewFamily& InViewFamily) override;
	virtual void SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView) override {}
	virtual void PreRenderViewFamily_RenderThread(FRDGBuilder& RHICmdList, FSceneViewFamily& InViewFamily) override;
	virtual void BeginRenderViewFamily(FSceneViewFamily& InViewFamily) override {}
	virtual bool IsActiveThisFrame_Internal(const FSceneViewExtensionContext& Context) const override { return true; }

	void SetBoidsData(int32 numBoids, TRefCountPtr<FRDGPooledBuffer> InBuffer);
	void DisposeBoidsBuffer();

private:
	FBoidsSceneUBtoMatSceneViewExt_GameThread BoidsSceneUBtoMatSceneViewExt_GT;
	FBoidsSceneUBtoMatSceneViewExt_RenderThread BoidsSceneUBtoMatSceneViewExt_RT;
};