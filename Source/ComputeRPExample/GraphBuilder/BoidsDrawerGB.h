// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#pragma once

#include "Data/BoidsLib.h"
#include "Utils/PingPongBuffer.h"
#include "Engine/TextureRenderTarget2D.h"

class FComputeShader_BoidsDrawer
{

public:

	static void InitBoidsDrawerExample_RenderThread(FRDGBuilder& GraphBuilder,
		FBoidsRDGStateData& BoidsRDGStateData,
		const TArray<FBoidItem>& BoidsArray,
		FPingPongBuffer& BoidsPingPongBuffer);

	static void ExecuteBoidsDrawerExample_RenderThread(FRDGBuilder& GraphBuilder,
		FBoidsRDGStateData& BoidsRDGStateData,
		const FBoidCurrentParameters& BoidCurrentParameters,
		FPingPongBuffer& BoidsPingPongBuffer,
		FRDGTextureUAVRef OutputTextureUAV,
		UTextureRenderTarget2D* RenderTarget);
};

class FPixelShader_BoidsDrawer
{
public:
	static void InitDrawToRenderTargetExample_RenderThread(FRDGBuilder& GraphBuilder,
		FBoidsRDGStateData& BoidsRDGStateData,
		const TArray<FBoidItem>& BoidsArray,
		UTextureRenderTarget2D* RenderTarget,
		FRDGTextureRef& RenderTargetTexture);

	static void DrawToRenderTargetExample_RenderThread(FRDGBuilder& GraphBuilder,
		FBoidsRDGStateData& BoidsRDGStateData,
		FRDGTextureSRVRef OutputTexture,
		UTextureRenderTarget2D* RenderTarget,
		FRDGTextureRef& RenderTargetTexture);
};