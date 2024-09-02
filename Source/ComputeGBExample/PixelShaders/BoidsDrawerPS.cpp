#include "BoidsDrawerPS.h"
#include "CommonRenderResources.h"
#include "TextureResource.h"
#include "RHIStaticStates.h"

// This will tell the engine to create the shader and where the shader entry point is.
//                            ShaderType            ShaderPath           Shader function name Type
IMPLEMENT_GLOBAL_SHADER(FPixelShaderExamplePS, "/ComputeExample/PS_BoidsTexture.usf", "MainPixelShader", SF_Pixel);

TGlobalResource<FSimpleScreenVertexBuffer> GSimpleScreenVertexBuffer;

bool FPixelShaderExamplePS::ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
{
	// This example shader uses wave operations, so it requires SM6.
	return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM6);
}

void FPixelShader_BoidsDrawer::InitDrawToRenderTargetExample_RenderThread(FRDGBuilder& GraphBuilder, const TArray<FBoidItem>& BoidsArray, UTextureRenderTarget2D* RenderTarget, FRDGTextureRef& RenderTargetTexture)
{
	// First, we must register our output render target resource with the graph.
	/*TRefCountPtr<IPooledRenderTarget> pooledRenderTarget = CreateRenderTarget(RenderTarget->GetResource()->GetTexture2DRHI(), TEXT("Draw Example RenderTarget"));
	RenderTargetTexture = GraphBuilder.RegisterExternalTexture(pooledRenderTarget);*/
}

void FPixelShader_BoidsDrawer::DrawToRenderTargetExample_RenderThread(FRDGBuilder& GraphBuilder, FRDGTextureSRVRef OutputTexture, UTextureRenderTarget2D* RenderTarget, FRDGTextureRef& RenderTargetTexture)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_ShaderPlugin_PixelShader); // Used to gather CPU profiling data for the UE4 session frontend
	SCOPED_DRAW_EVENT(GraphBuilder.RHICmdList, ShaderPlugin_Pixel); // Used to profile GPU activity and add metadata to be consumed by for example RenderDoc

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
	GraphBuilder.AddPass(
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