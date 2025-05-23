// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#include "ComputeShaders/ST_BoidsRPCS.h"
#include "DataDrivenShaderPlatformInfo.h"

#define BoidsExample_ThreadsPerGroup 512

// This will tell the engine to create the shader and where the shader entry point is.
//                            ShaderType            ShaderPath           Shader function name Type
IMPLEMENT_GLOBAL_SHADER(FST_BoidsRPInitExampleCS, "/ComputeCore/Private/ComputeRPExample/CS_Boids.usf", "GenerateBoids", SF_Compute);

bool FST_BoidsRPInitExampleCS::ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
{
	// This example shader uses wave operations, so it requires SM6.
	return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM6);
}

void FST_BoidsRPInitExampleCS::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);

	OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_X"), BoidsExample_ThreadsPerGroup);
	OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_Y"), 1);
	OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_Z"), 1);
}

// This will tell the engine to create the shader and where the shader entry point is.
//                            ShaderType            ShaderPath           Shader function name Type
IMPLEMENT_GLOBAL_SHADER(FST_BoidsRPUpdateExampleCS, "/ComputeCore/Private/ComputeRPExample/CS_Boids.usf", "UpdateBoids", SF_Compute);

bool FST_BoidsRPUpdateExampleCS::ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
{
	// This example shader uses wave operations, so it requires SM6.
	return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM6);
}

void FST_BoidsRPUpdateExampleCS::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);

	OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_X"), BoidsExample_ThreadsPerGroup);
	OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_Y"), 1);
	OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_Z"), 1);
}

// This will tell the engine to create the shader and where the shader entry point is.
//                            ShaderType            ShaderPath           Shader function name Type
IMPLEMENT_GLOBAL_SHADER(FST_BoidsRPCopyBufferExampleCS, "/ComputeCore/Private/ComputeRPExample/CS_BoidsCopyBuffer.usf", "CopyBuffer", SF_Compute);

bool FST_BoidsRPCopyBufferExampleCS::ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
{
	// This example shader uses wave operations, so it requires SM6.
	return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM6);
}

void FST_BoidsRPCopyBufferExampleCS::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);

	OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_X"), BoidsExample_ThreadsPerGroup);
	OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_Y"), 1);
	OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_Z"), 1);
}