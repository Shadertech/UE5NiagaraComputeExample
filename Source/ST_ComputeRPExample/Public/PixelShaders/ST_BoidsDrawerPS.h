// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ST_ComputeRPExample.h"
#include "GlobalShader.h"
#include "Data/ST_BoidsLib.h"
#include "Utils/ST_PingPongBuffer.h"
#include "VertexShaders/ST_BoidsDrawerVS.h"
#include "Engine/TextureRenderTarget2D.h"
#include "DataDrivenShaderPlatformInfo.h"

/**********************************************************************************************/
/* This class carries our parameter declarations and acts as the bridge between cpp and HLSL. */
/**********************************************************************************************/
class FST_PixelShaderExamplePS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FST_PixelShaderExamplePS);
	SHADER_USE_PARAMETER_STRUCT(FST_PixelShaderExamplePS, FGlobalShader);

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
BEGIN_SHADER_PARAMETER_STRUCT(FST_BoidsDrawExampleParameters, )
	SHADER_PARAMETER_STRUCT_INCLUDE(FST_SimplePassThroughVS::FParameters, VS)
	SHADER_PARAMETER_STRUCT_INCLUDE(FST_PixelShaderExamplePS::FParameters, PS)
	RENDER_TARGET_BINDING_SLOTS()
END_SHADER_PARAMETER_STRUCT()