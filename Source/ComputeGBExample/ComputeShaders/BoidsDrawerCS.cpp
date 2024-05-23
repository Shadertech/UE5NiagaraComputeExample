#include "BoidsDrawerCS.h"
#include "RenderGraphUtils.h"

#define BoidsDrawerExample_ThreadsPerGroup 32

// This will tell the engine to create the shader and where the shader entry point is.
//                            ShaderType            ShaderPath           Shader function name Type
IMPLEMENT_GLOBAL_SHADER(FBoidsGBDrawerExampleCS, "/ComputeExample/CS_BoidsTexture.usf", "DrawBoids", SF_Compute);

bool FBoidsGBDrawerExampleCS::ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
{
	// This example shader uses wave operations, so it requires SM6.
	return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM6);
}

void FBoidsGBDrawerExampleCS::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);

	OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_X"), BoidsDrawerExample_ThreadsPerGroup);
	OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_Y"), BoidsDrawerExample_ThreadsPerGroup);
	OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_Z"), 1);
}

/**********************************************************************************************/
/* These functions schedule our Compute Shader work from the CPU!							  */
/**********************************************************************************************/

void FComputeShader_BoidsDrawer::InitBoidsDrawerExample_RenderThread(FRDGBuilder& GraphBuilder, const TArray<FBoidItem>& BoidsArray, FPingPongBuffer& BoidsPingPongBuffer)
{
	//FRDGTextureRef OutputTexture = RDGBuilder.CreateTexture(ComputeShaderOutputDesc, TEXT("ShaderPlugin_ComputeShaderOutput"), ERDGTextureFlags::None);
}

void FComputeShader_BoidsDrawer::DispatchBoidsDrawerExample_RenderThread(FRDGBuilder& GraphBuilder, const FBoidConstantParameters& BoidConstantParameters, FPingPongBuffer& BoidsPingPongBuffer, FRDGTextureUAVRef OutputTextureUAV, UTextureRenderTarget2D* RenderTarget)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_ComputeGBExample_Dispatch);
	SCOPED_DRAW_EVENT(GraphBuilder.RHICmdList, ComputeGBExample_Compute); // Used to profile GPU activity and add metadata to be consumed by for example RenderDoc

	// This shader shows how you can use a compute shader to write to a texture.
	// Here we send our inputs using parameters as part of the shader parameter struct. This is an efficient way of sending in simple constants for a shader, but does not work well if you need to send larger amounts of data.
	FBoidsGBDrawerExampleCS::FParameters* PassParameters = GraphBuilder.AllocParameters<FBoidsGBDrawerExampleCS::FParameters>();
	PassParameters->OutputTexture = OutputTextureUAV;
	PassParameters->numBoids = BoidConstantParameters.numBoids;
	PassParameters->boidsIn = BoidsPingPongBuffer.ReadScopedSRV;
	PassParameters->textureSize = FVector2f(RenderTarget->SizeX, RenderTarget->SizeY);

	TShaderMapRef<FBoidsGBDrawerExampleCS> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

	//int32 size = BoidConstantParameters.numBoids / 2;
	FIntVector GroupCounts = FIntVector(FMath::DivideAndRoundUp(RenderTarget->SizeX, BoidsDrawerExample_ThreadsPerGroup), FMath::DivideAndRoundUp(RenderTarget->SizeY, BoidsDrawerExample_ThreadsPerGroup), 1);
	FComputeShaderUtils::AddPass(GraphBuilder, RDG_EVENT_NAME("BoidsDrawerExample"), ERDGPassFlags::Compute | ERDGPassFlags::NeverCull, ComputeShader, PassParameters, GroupCounts);
}