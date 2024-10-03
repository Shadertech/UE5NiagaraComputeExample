#pragma once

#include "Data/BoidsLib.h"
#include "GlobalShader.h"
#include "ShaderParameterStruct.h"
#include "ShaderParameterMacros.h"
#include "SceneUniformBuffer.h"

class FBoidsRPFlattenExampleCS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FBoidsRPFlattenExampleCS);
	SHADER_USE_PARAMETER_STRUCT(FBoidsRPFlattenExampleCS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<FBoidItem>, boidsIn)
		SHADER_PARAMETER_RDG_BUFFER_UAV(RWStructuredBuffer<float>, flatpackBoidsOut)
		SHADER_PARAMETER(uint32, numBoids)
	END_SHADER_PARAMETER_STRUCT()

public:
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters);
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);
};

BEGIN_SHADER_PARAMETER_STRUCT(FExampleSceneUniformBufferParams, )
	//SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<FBoidItem>, boidsIn)
	SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<float>, boidsIn)
	SHADER_PARAMETER(int32, numBoids)
END_SHADER_PARAMETER_STRUCT()

DECLARE_SCENE_UB_STRUCT(FExampleSceneUniformBufferParams, ComputeExample, )

BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FBoidsRPFlattenExampleParams, )
	//SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<FBoidItem>, boidsIn)
	//SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<float>, boidsIn)
	SHADER_PARAMETER(int32, numBoids)
END_GLOBAL_SHADER_PARAMETER_STRUCT()