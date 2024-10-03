#pragma once

#include "CoreMinimal.h"
#include "ComputeRPExample.h"
#include "GlobalShader.h"
#include "Data/BoidsLib.h"
#include "Utils/PingPongBuffer.h"
#include "VertexShaders/BoidsDrawerVS.h"
#include "Engine/TextureRenderTarget2D.h"
#include "DataDrivenShaderPlatformInfo.h"

/**********************************************************************************************/
/* This class carries our parameter declarations and acts as the bridge between cpp and HLSL. */
/**********************************************************************************************/
class FPixelShaderExamplePS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FPixelShaderExamplePS);
	SHADER_USE_PARAMETER_STRUCT(FPixelShaderExamplePS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		//SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<uint>, ComputeShaderOutput)
		SHADER_PARAMETER_RDG_TEXTURE_SRV(Texture2D<float4>, ComputeShaderOutput)
		/*SHADER_PARAMETER(FVector4f, StartColor)
		SHADER_PARAMETER(FVector4f, EndColor)*/
		SHADER_PARAMETER(FVector2f, TextureSize) // Metal doesn't support GetDimensions(), so we send in this data via our parameters.
		//SHADER_PARAMETER(float, BlendFactor)
	END_SHADER_PARAMETER_STRUCT()

public:
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters);
};

// Since we have to allocate the params via the RenderGraph, it's practical to use a wrapper struct like this for both VS and PS inputs.
// We also have to include binding slots in our parameter struct, as this is what we will be rendering to, and this wrapper struct is perfect for that!
BEGIN_SHADER_PARAMETER_STRUCT(FBoidsDrawExampleParameters, )
	SHADER_PARAMETER_STRUCT_INCLUDE(FSimplePassThroughVS::FParameters, VS)
	SHADER_PARAMETER_STRUCT_INCLUDE(FPixelShaderExamplePS::FParameters, PS)
	RENDER_TARGET_BINDING_SLOTS()
END_SHADER_PARAMETER_STRUCT()

class FPixelShader_BoidsDrawer
{
public:
	static void InitDrawToRenderTargetExample_RenderThread(FRDGBuilder& GraphBuilder, const TArray<FBoidItem>& BoidsArray, UTextureRenderTarget2D* RenderTarget, FRDGTextureRef& RenderTargetTexture);
	static void DrawToRenderTargetExample_RenderThread(FRDGBuilder& GraphBuilder, FRDGTextureSRVRef OutputTexture, UTextureRenderTarget2D* RenderTarget, FRDGTextureRef& RenderTargetTexture);
};