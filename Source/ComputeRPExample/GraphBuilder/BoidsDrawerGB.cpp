#include "BoidsDrawerGB.h"
#include "RenderGraphUtils.h"
#include "ComputeShaders/BoidsRPDrawerCS.h"
#include "PixelShaders/BoidsDrawerPS.h"
#include "CommonRenderResources.h"
#include "TextureResource.h"

#define BoidsDrawerExample_ThreadsPerGroup 32

/**********************************************************************************************/
/* These functions schedule our Compute Shader work from the CPU!							  */
/**********************************************************************************************/

void FComputeShader_BoidsDrawer::InitBoidsDrawerExample_RenderThread(FRDGBuilder& GraphBuilder, FBoidsRDGStateData& BoidsRDGStateData, const TArray<FBoidItem>& BoidsArray, FPingPongBuffer& BoidsPingPongBuffer)
{
	//FRDGTextureRef OutputTexture = RDGBuilder.CreateTexture(ComputeShaderOutputDesc, TEXT("ShaderPlugin_ComputeShaderOutput"), ERDGTextureFlags::None);
}

void FComputeShader_BoidsDrawer::ExecuteBoidsDrawerExample_RenderThread(FRDGBuilder& GraphBuilder, FBoidsRDGStateData& BoidsRDGStateData, const FBoidCurrentParameters& BoidCurrentParameters, FPingPongBuffer& BoidsPingPongBuffer, FRDGTextureUAVRef OutputTextureUAV, UTextureRenderTarget2D* RenderTarget)
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
	BoidsRDGStateData.ExecutePass[1] = FComputeShaderUtils::AddPass(GraphBuilder, RDG_EVENT_NAME("BoidsDrawerExample"), ERDGPassFlags::Compute | ERDGPassFlags::NeverCull, ComputeShader, PassParameters, GroupCounts);
}

TGlobalResource<FSimpleScreenVertexBuffer> GSimpleScreenVertexBuffer;

void FPixelShader_BoidsDrawer::InitDrawToRenderTargetExample_RenderThread(FRDGBuilder& GraphBuilder, FBoidsRDGStateData& BoidsRDGStateData, const TArray<FBoidItem>& BoidsArray, UTextureRenderTarget2D* RenderTarget, FRDGTextureRef& RenderTargetTexture)
{
	// First, we must register our output render target resource with the graph.
	/*TRefCountPtr<IPooledRenderTarget> pooledRenderTarget = CreateRenderTarget(RenderTarget->GetResource()->GetTexture2DRHI(), TEXT("Draw Example RenderTarget"));
	RenderTargetTexture = GraphBuilder.RegisterExternalTexture(pooledRenderTarget);*/
}

void FPixelShader_BoidsDrawer::DrawToRenderTargetExample_RenderThread(FRDGBuilder& GraphBuilder, FBoidsRDGStateData& BoidsRDGStateData, FRDGTextureSRVRef OutputTexture, UTextureRenderTarget2D* RenderTarget, FRDGTextureRef& RenderTargetTexture)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_BoidsDrawer_Pixel); // Used to gather CPU profiling data for the UE4 session frontend
	SCOPED_DRAW_EVENT(GraphBuilder.RHICmdList, BoidsDrawer_Pixel); // Used to profile GPU activity and add metadata to be consumed by for example RenderDoc

	// First, we must register our output render target resource with the graph.
	TRefCountPtr<IPooledRenderTarget> pooledRenderTarget = CreateRenderTarget(RenderTarget->GetResource()->GetTexture2DRHI(), TEXT("Draw Example RenderTarget"));
	RenderTargetTexture = GraphBuilder.RegisterExternalTexture(pooledRenderTarget);

	FBoidsDrawExampleParameters* ShaderParams = GraphBuilder.AllocParameters<FBoidsDrawExampleParameters>();
	ShaderParams->RenderTargets[0] = FRenderTargetBinding(RenderTargetTexture, ERenderTargetLoadAction::ELoad);
	ShaderParams->PS.ComputeShaderOutput = OutputTexture;
	//ShaderParams->PS.StartColor = FVector4f(DrawParameters.StartColor.R, DrawParameters.StartColor.G, DrawParameters.StartColor.B, DrawParameters.StartColor.A) / 255.0f;
	//ShaderParams->PS.EndColor = FVector4f(DrawParameters.EndColor.R, DrawParameters.EndColor.G, DrawParameters.EndColor.B, DrawParameters.EndColor.A) / 255.0f;

	ShaderParams->PS.TextureSize = FVector2f(RenderTarget->SizeX, RenderTarget->SizeY);
	//ShaderParams->PS.BlendFactor = DrawParameters.ComputeShaderBlend;

	auto ShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);
	TShaderMapRef<FSimplePassThroughVS> VertexShader(ShaderMap);
	TShaderMapRef<FPixelShaderExamplePS> PixelShader(ShaderMap);

	// If we want to use a vertex shader like this, we have to schedule a custom pass!
	BoidsRDGStateData.ExecutePass[2] = GraphBuilder.AddPass(
		RDG_EVENT_NAME("FPixelShaderExample"),
		ShaderParams,
		ERDGPassFlags::Raster,
		[ShaderParams, VertexShader, PixelShader](FRHICommandList& RHICmdList)
		{
			// Set the graphic pipeline state.
			FGraphicsPipelineStateInitializer GraphicsPSOInit;
			RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);
			GraphicsPSOInit.BlendState = TStaticBlendState<>::GetRHI();
			GraphicsPSOInit.RasterizerState = TStaticRasterizerState<>::GetRHI();
			GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();
			GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GFilterVertexDeclaration.VertexDeclarationRHI;
			GraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
			GraphicsPSOInit.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader();
			GraphicsPSOInit.PrimitiveType = PT_TriangleStrip;
			SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit, 0);

			// Set parameters
			SetShaderParameters(RHICmdList, PixelShader, PixelShader.GetPixelShader(), ShaderParams->PS);
			SetShaderParameters(RHICmdList, VertexShader, VertexShader.GetVertexShader(), ShaderParams->VS);

			// Draw
			RHICmdList.SetStreamSource(0, GSimpleScreenVertexBuffer.VertexBufferRHI, 0);
			RHICmdList.DrawPrimitive(0, 2, 1);
		});
}