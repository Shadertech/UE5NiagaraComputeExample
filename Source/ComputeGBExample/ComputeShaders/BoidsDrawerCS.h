#pragma once

#include "CoreMinimal.h"
#include "Data/BoidsLib.h"
#include "Utils/PingPongBuffer.h"
#include "ComputeGBExample.h"
#include "GlobalShader.h"
#include "Engine/TextureRenderTarget2D.h"
#include "DataDrivenShaderPlatformInfo.h"

class FBoidsDrawerExampleCS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FBoidsDrawerExampleCS);
	SHADER_USE_PARAMETER_STRUCT(FBoidsDrawerExampleCS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, OutputTexture)
		//SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<uint>, OutputTexture)
		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<FBoidItem>, boidsIn)
		SHADER_PARAMETER(uint32, numBoids)
		SHADER_PARAMETER(FVector2f, textureSize)
	END_SHADER_PARAMETER_STRUCT()

public:
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters);
	static inline void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);
};

// This will tell the engine to create the shader and where the shader entry point is.
//                            ShaderType            ShaderPath           Shader function name Type
IMPLEMENT_GLOBAL_SHADER(FBoidsDrawerExampleCS, "/ComputeExample/CS_BoidsTexture.usf", "DrawBoids", SF_Compute);

class FComputeShader_BoidsDrawer
{
public:
	static void InitBoidsDrawerExample_RenderThread(FRDGBuilder& GraphBuilder, const TArray<FBoidItem>& BoidsArray, FPingPongBuffer& BoidsPingPongBuffer);
	static void DispatchBoidsDrawerExample_RenderThread(FRDGBuilder& GraphBuilder, const FBoidConstantParameters& BoidConstantParameters, FPingPongBuffer& BoidsPingPongBuffer, FRDGTextureUAVRef OutputTextureUAV, UTextureRenderTarget2D* RenderTarget);
	static void DrawToRenderTarget_RenderThread(FRDGBuilder& GraphBuilder, const FBoidConstantParameters& BoidConstantParameters, FRDGTextureSRVRef OutputTexture, UTextureRenderTarget2D* RenderTarget);
};