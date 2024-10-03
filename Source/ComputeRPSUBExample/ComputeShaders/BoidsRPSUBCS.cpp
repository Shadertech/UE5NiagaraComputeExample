#include "BoidsRPSUBCS.h"
#include "RenderGraphBuilder.h"
#include "Utils/ComputeFunctionLibrary.h"
#include "DataDrivenShaderPlatformInfo.h"
#include "SceneUniformBuffer.h"

#define FlattenBoidsExample_ThreadsPerGroup 512

// This will tell the engine to create the shader and where the shader entry point is.
//                            ShaderType            ShaderPath           Shader function name Type
IMPLEMENT_GLOBAL_SHADER(FBoidsRPFlattenExampleCS, "/ComputeCore/ComputeRPExample/CS_Boids.usf", "FlattenBoids", SF_Compute);

bool FBoidsRPFlattenExampleCS::ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
{
	// This example shader uses wave operations, so it requires SM6.
	return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM6);
}

void FBoidsRPFlattenExampleCS::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);

	OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_X"), FlattenBoidsExample_ThreadsPerGroup);
	OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_Y"), 1);
	OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_Z"), 1);
}

static void GetDefaultResourceParameters_ExampleUB(FExampleSceneUniformBufferParams& ShaderParams, FRDGBuilder& GraphBuilder)
{
	ShaderParams.numBoids = 0;
	FRDGBufferRef DummyBuffer = UComputeFunctionLibrary::CreateStructuredBuffer(GraphBuilder, TEXT("DummyBoidItemBuffer"), 4, 1);
	ShaderParams.boidsIn = GraphBuilder.CreateSRV(DummyBuffer);
}

IMPLEMENT_SCENE_UB_STRUCT(FExampleSceneUniformBufferParams, ComputeExample, GetDefaultResourceParameters_ExampleUB);

IMPLEMENT_GLOBAL_SHADER_PARAMETER_STRUCT(FBoidsRPFlattenExampleParams, "BoidsRPFlattenExampleParams");