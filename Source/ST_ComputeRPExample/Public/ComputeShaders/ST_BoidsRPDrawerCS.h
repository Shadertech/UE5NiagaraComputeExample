// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#pragma once

#include "Data/ST_BoidsLib.h"
#include "GlobalShader.h"
#include "ShaderParameterStruct.h"

class FST_BoidsRPDrawerExampleCS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FST_BoidsRPDrawerExampleCS);
	SHADER_USE_PARAMETER_STRUCT(FST_BoidsRPDrawerExampleCS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, OutputTexture)
		//SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<uint>, OutputTexture)
		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<FST_BoidItem>, boidsIn)
		SHADER_PARAMETER(uint32, numBoids)
		SHADER_PARAMETER(FVector2f, textureSize)
	END_SHADER_PARAMETER_STRUCT()

public:
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters);
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);
};