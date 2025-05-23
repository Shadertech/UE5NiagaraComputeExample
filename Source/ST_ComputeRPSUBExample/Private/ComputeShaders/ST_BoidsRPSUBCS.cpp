// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#include "ComputeShaders/ST_BoidsRPSUBCS.h"
#include "RenderGraphBuilder.h"
#include "Utils/ST_ComputeFunctionLibrary.h"
#include "SystemTextures.h"

static void GetDefaultResourceParameters_ExampleUB(FST_ExampleSceneUniformBufferParams& ShaderParams, FRDGBuilder& GraphBuilder)
{
	ShaderParams.numBoids = 0;
	FRDGBufferRef DummyBuffer = GSystemTextures.GetDefaultStructuredBuffer(GraphBuilder, 4);
	ShaderParams.boidsIn = GraphBuilder.CreateSRV(DummyBuffer);
}

IMPLEMENT_SCENE_UB_STRUCT(FST_ExampleSceneUniformBufferParams, ComputeExample, GetDefaultResourceParameters_ExampleUB);

//IMPLEMENT_GLOBAL_SHADER_PARAMETER_STRUCT(FBoidsRPFlattenExampleParams, "BoidsRPFlattenExampleParams");