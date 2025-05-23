// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#pragma once

#include "Data/ST_BoidsLib.h"
#include "Utils/ST_PingPongBuffer.h"
#include "Engine/TextureRenderTarget2D.h"

class FST_ComputeShader_BoidsDrawer
{

public:

	static void InitBoidsDrawerExample_RenderThread(FRDGBuilder& GraphBuilder,
		FST_BoidsRDGStateData& BoidsRDGStateData,
		const TArray<FST_BoidItem>& BoidsArray,
		FST_PingPongBuffer& BoidsPingPongBuffer);

	static void ExecuteBoidsDrawerExample_RenderThread(FRDGBuilder& GraphBuilder,
		FST_BoidsRDGStateData& BoidsRDGStateData,
		const FST_BoidCurrentParameters& BoidCurrentParameters,
		FST_PingPongBuffer& BoidsPingPongBuffer,
		FRDGTextureUAVRef OutputTextureUAV,
		UTextureRenderTarget2D* RenderTarget);
};

class FST_PixelShader_BoidsDrawer
{
public:
	static void InitDrawToRenderTargetExample_RenderThread(FRDGBuilder& GraphBuilder,
		FST_BoidsRDGStateData& BoidsRDGStateData,
		const TArray<FST_BoidItem>& BoidsArray,
		UTextureRenderTarget2D* RenderTarget,
		FRDGTextureRef& RenderTargetTexture);

	static void DrawToRenderTargetExample_RenderThread(FRDGBuilder& GraphBuilder,
		FST_BoidsRDGStateData& BoidsRDGStateData,
		FRDGTextureSRVRef OutputTexture,
		UTextureRenderTarget2D* RenderTarget,
		FRDGTextureRef& RenderTargetTexture);
};