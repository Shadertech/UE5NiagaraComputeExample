#pragma once

#include "Data/BoidsLib.h"
#include "Utils/PingPongBuffer.h"
#include "Engine/TextureRenderTarget2D.h"

class FComputeShader_BoidsDrawer
{

public:

	static void InitBoidsDrawerExample_RenderThread(FRDGBuilder& GraphBuilder,
		const TArray<FBoidItem>& BoidsArray,
		FPingPongBuffer& BoidsPingPongBuffer);

	static void ExecuteBoidsDrawerExample_RenderThread(FRDGBuilder& GraphBuilder,
		const FBoidCurrentParameters& BoidCurrentParameters,
		FPingPongBuffer& BoidsPingPongBuffer,
		FRDGTextureUAVRef OutputTextureUAV,
		UTextureRenderTarget2D* RenderTarget);

	static void DrawToRenderTarget_RenderThread(FRDGBuilder& GraphBuilder,
		FRDGTextureSRVRef OutputTexture,
		UTextureRenderTarget2D* RenderTarget);
};