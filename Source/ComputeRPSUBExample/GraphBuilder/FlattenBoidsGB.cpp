#include "GraphBuilder/FlattenBoidsGB.h"
#include "RenderGraphResources.h"
#include "RenderGraphBuilder.h"
#include "Utils/ComputeFunctionLibrary.h"
#include "RenderGraphUtils.h"
#include "ComputeShaders/BoidsRPSUBCS.h"

#define FlattenBoidsExample_ThreadsPerGroup 512

void FGraphBullder_FlattenBoids::InitFlattenBoidsExample_RenderThread(FRDGBuilder& GraphBuilder, const TCHAR* OwnerName, const TArray<FBoidItem>& BoidsArray, FBoidsRDGStateData& BoidsRDGStateData, FPingPongBuffer& FlattenBoidsPingPongBuffer, FPingPongBuffer& BoidsPingPongBuffer)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_FlattenBoidsExample_Init);
	SCOPED_DRAW_EVENT(GraphBuilder.RHICmdList, FlattenBoidsExample_Init);

	const TCHAR* WriteBufferName = *(FString(OwnerName) + TEXT("FlattenBoidsOutBuffer"));
	const TCHAR* UAVName = *(FString(OwnerName) + TEXT("FlattenBoidsOut_StructuredBuffer"));

	FRDGBufferRef WriteBuffer = UComputeFunctionLibrary::CreateStructuredBuffer(GraphBuilder, WriteBufferName, 4, BoidsArray.Num() * 12);
	FlattenBoidsPingPongBuffer.WritePooled = GraphBuilder.ConvertToExternalBuffer(WriteBuffer);
	FlattenBoidsPingPongBuffer.RegisterW(GraphBuilder, WriteBufferName);

	//GraphBuilder.QueueBufferExtraction(BoidsPingPongBuffer.WriteScopedRef, &BoidsPingPongBuffer.ReadPooled);
}

void FGraphBullder_FlattenBoids::ExecuteFlattenBoidsExample_RenderThread(FRDGBuilder& GraphBuilder, const TCHAR* OwnerName, const FBoidCurrentParameters& BoidCurrentParameters, FBoidsRDGStateData& BoidsRDGStateData, FPingPongBuffer& FlattenBoidsPingPongBuffer, FPingPongBuffer& BoidsPingPongBuffer, float DeltaTime)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_FlattenBoidsExample_Execute);
	SCOPED_DRAW_EVENT(GraphBuilder.RHICmdList, FlattenBoidsExample_Execute);

	/*BoidsRDGStateData.ExecutePass[1] = UComputeFunctionLibrary::AddCopyBufferPass(GraphBuilder, BoidsPingPongBuffer.WriteScopedRef, BoidsPingPongBuffer.ReadScopedRef);
	GraphBuilder.AddPassDependency(BoidsRDGStateData.ExecutePass[0], BoidsRDGStateData.ExecutePass[1]);*/

	/*const TCHAR* SRVName = *(FString(OwnerName) + TEXT("BoidsIn_StructuredBuffer"));
	BoidsPingPongBuffer.RegisterR(GraphBuilder, SRVName);*/

	const TCHAR* UAVName = *(FString(OwnerName) + TEXT("FlattenBoidsOut_StructuredBuffer"));
	FlattenBoidsPingPongBuffer.RegisterW(GraphBuilder, UAVName);

	FBoidsRPFlattenExampleCS::FParameters* PassParameters = GraphBuilder.AllocParameters<FBoidsRPFlattenExampleCS::FParameters>();
	PassParameters->numBoids = BoidCurrentParameters.ConstantParameters.numBoids;
	PassParameters->boidsIn = BoidsPingPongBuffer.ReadScopedSRV;
	PassParameters->flatpackBoidsOut = FlattenBoidsPingPongBuffer.WriteScopedUAV;

	TShaderMapRef<FBoidsRPFlattenExampleCS> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	FIntVector GroupCounts = FIntVector(FMath::DivideAndRoundUp(BoidCurrentParameters.ConstantParameters.numBoids, FlattenBoidsExample_ThreadsPerGroup), 1, 1);
	BoidsRDGStateData.ExecutePass[1] = FComputeShaderUtils::AddPass(GraphBuilder,
		RDG_EVENT_NAME("RP_FlattenBoidsUpdateExample"),
		ERDGPassFlags::Compute | ERDGPassFlags::NeverCull,
		ComputeShader,
		PassParameters,
		GroupCounts);

	//GraphBuilder.AddPassDependency(BoidsRDGStateData.ExecutePass[0], BoidsRDGStateData.ExecutePass[1]);

	GraphBuilder.QueueBufferExtraction(FlattenBoidsPingPongBuffer.WriteScopedRef, &FlattenBoidsPingPongBuffer.ReadPooled);
}