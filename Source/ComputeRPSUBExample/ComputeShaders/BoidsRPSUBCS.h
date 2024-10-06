#pragma once

#include "Data/BoidsLib.h"
#include "ShaderParameterStruct.h"
#include "ShaderParameterMacros.h"
#include "SceneUniformBuffer.h"

BEGIN_SHADER_PARAMETER_STRUCT(FExampleSceneUniformBufferParams, )
	//SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<FBoidItem>, boidsIn)
	SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<float>, boidsIn)
	SHADER_PARAMETER(int32, numBoids)
END_SHADER_PARAMETER_STRUCT()

DECLARE_SCENE_UB_STRUCT(FExampleSceneUniformBufferParams, ComputeExample, )

//BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FBoidsRPFlattenExampleParams, )
//	//SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<FBoidItem>, boidsIn)
//	//SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<float>, boidsIn)
//	SHADER_PARAMETER(int32, numBoids)
//END_GLOBAL_SHADER_PARAMETER_STRUCT()