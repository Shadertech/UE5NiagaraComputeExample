// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#include "BoidsRPSUBCS.h"
#include "RenderGraphBuilder.h"
#include "Utils/ComputeFunctionLibrary.h"
#include "SystemTextures.h"

static void GetDefaultResourceParameters_ExampleUB(FExampleSceneUniformBufferParams& ShaderParams, FRDGBuilder& GraphBuilder)
{
	ShaderParams.numBoids = 0;
	FRDGBufferRef DummyBuffer = GSystemTextures.GetDefaultStructuredBuffer(GraphBuilder, 4);
	ShaderParams.boidsIn = GraphBuilder.CreateSRV(DummyBuffer);
}

IMPLEMENT_SCENE_UB_STRUCT(FExampleSceneUniformBufferParams, ComputeExample, GetDefaultResourceParameters_ExampleUB);

//IMPLEMENT_GLOBAL_SHADER_PARAMETER_STRUCT(FBoidsRPFlattenExampleParams, "BoidsRPFlattenExampleParams");