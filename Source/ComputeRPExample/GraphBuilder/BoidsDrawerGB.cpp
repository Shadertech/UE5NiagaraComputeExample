#include "BoidsDrawerGB.h"
#include "RenderGraphUtils.h"
#include "ComputeShaders/BoidsRPDrawerCS.h"

#define BoidsDrawerExample_ThreadsPerGroup 32

/**********************************************************************************************/
/* These functions schedule our Compute Shader work from the CPU!							  */
/**********************************************************************************************/

void FComputeShader_BoidsDrawer::InitBoidsDrawerExample_RenderThread(FRDGBuilder& GraphBuilder, const TArray<FBoidItem>& BoidsArray, FPingPongBuffer& BoidsPingPongBuffer)
{
	//FRDGTextureRef OutputTexture = RDGBuilder.CreateTexture(ComputeShaderOutputDesc, TEXT("ShaderPlugin_ComputeShaderOutput"), ERDGTextureFlags::None);
}

void FComputeShader_BoidsDrawer::ExecuteBoidsDrawerExample_RenderThread(FRDGBuilder& GraphBuilder, const FBoidCurrentParameters& BoidCurrentParameters, FPingPongBuffer& BoidsPingPongBuffer, FRDGTextureUAVRef OutputTextureUAV, UTextureRenderTarget2D* RenderTarget)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_BoidsDrawer_Execute);
	SCOPED_DRAW_EVENT(GraphBuilder.RHICmdList, BoidsDrawer_Execute);

	// This shader shows how you can use a compute shader to write to a texture.
	// Here we send our inputs using parameters as part of the shader parameter struct. This is an efficient way of sending in simple constants for a shader, but does not work well if you need to send larger amounts of data.
	FBoidsRPDrawerExampleCS::FParameters* PassParameters = GraphBuilder.AllocParameters<FBoidsRPDrawerExampleCS::FParameters>();
	PassParameters->OutputTexture = OutputTextureUAV;
	PassParameters->numBoids = BoidCurrentParameters.ConstantParameters.numBoids;
	PassParameters->boidsIn = BoidsPingPongBuffer.ReadScopedSRV;
	PassParameters->textureSize = FVector2f(RenderTarget->SizeX, RenderTarget->SizeY);

	TShaderMapRef<FBoidsRPDrawerExampleCS> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

	//int32 size = BoidConstantParameters.numBoids / 2;
	FIntVector GroupCounts = FIntVector(FMath::DivideAndRoundUp(RenderTarget->SizeX, BoidsDrawerExample_ThreadsPerGroup), FMath::DivideAndRoundUp(RenderTarget->SizeY, BoidsDrawerExample_ThreadsPerGroup), 1);
	FComputeShaderUtils::AddPass(GraphBuilder, RDG_EVENT_NAME("BoidsDrawerExample"), ERDGPassFlags::Compute | ERDGPassFlags::NeverCull, ComputeShader, PassParameters, GroupCounts);
}