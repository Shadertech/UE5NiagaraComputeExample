#pragma once

#include "Data/BoidsLib.h"
#include "GlobalShader.h"
#include "ShaderParameterStruct.h"

class FBoidsRPDrawerExampleCS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FBoidsRPDrawerExampleCS);
	SHADER_USE_PARAMETER_STRUCT(FBoidsRPDrawerExampleCS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, OutputTexture)
		//SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<uint>, OutputTexture)
		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<FBoidItem>, boidsIn)
		SHADER_PARAMETER(uint32, numBoids)
		SHADER_PARAMETER(FVector2f, textureSize)
	END_SHADER_PARAMETER_STRUCT()

public:
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters);
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);
};