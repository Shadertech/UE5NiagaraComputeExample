// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#include "BoidsDrawerPS.h"
#include "RHIStaticStates.h"

// This will tell the engine to create the shader and where the shader entry point is.
//                            ShaderType            ShaderPath           Shader function name Type
IMPLEMENT_GLOBAL_SHADER(FPixelShaderExamplePS, "/ComputeCore/ComputeRPExample/PS_BoidsTexture.usf", "MainPixelShader", SF_Pixel);

bool FPixelShaderExamplePS::ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
{
	// This example shader uses wave operations, so it requires SM6.
	return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM6);
}