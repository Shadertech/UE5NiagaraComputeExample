// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#include "GraphBuilder/ST_FlattenBoidsGB.h"
#include "RenderGraphResources.h"
#include "RenderGraphBuilder.h"
#include "Utils/ST_ComputeFunctionLibrary.h"
#include "RenderGraphUtils.h"
#include "ComputeShaders/ST_BoidsRPSUBCS.h"

void FST_GraphBullder_FlattenBoids::InitFlattenBoidsExample_RenderThread(FRDGBuilder& GraphBuilder, const TCHAR* OwnerName, const TArray<FST_BoidItem>& BoidsArray, FST_BoidsRDGStateData& BoidsRDGStateData, FST_PingPongBuffer& FlattenBoidsPingPongBuffer, FST_PingPongBuffer& BoidsPingPongBuffer)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_FlattenBoidsExample_Init);
	SCOPED_DRAW_EVENT(GraphBuilder.RHICmdList, FlattenBoidsExample_Init);

	const TCHAR* WriteBufferName = *(FString(OwnerName) + TEXT("FlattenBoidsOutBuffer"));

	FRDGBufferRef WriteBuffer = UST_ComputeFunctionLibrary::CreateStructuredBuffer(GraphBuilder, WriteBufferName, 4, BoidsArray.Num() * 12);
	FlattenBoidsPingPongBuffer.WritePooled = GraphBuilder.ConvertToExternalBuffer(WriteBuffer);
}

void FST_GraphBullder_FlattenBoids::ExecuteFlattenBoidsExample_RenderThread(FRDGBuilder& GraphBuilder, const TCHAR* OwnerName, const FST_BoidCurrentParameters& BoidCurrentParameters, FST_BoidsRDGStateData& BoidsRDGStateData, FST_PingPongBuffer& FlattenBoidsPingPongBuffer, FST_PingPongBuffer& BoidsPingPongBuffer, float DeltaTime)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_FlattenBoidsExample_Execute);
	SCOPED_DRAW_EVENT(GraphBuilder.RHICmdList, FlattenBoidsExample_Execute);

	const TCHAR* UAVName = *(FString(OwnerName) + TEXT("FlattenBoidsOut_StructuredBuffer"));
	FlattenBoidsPingPongBuffer.RegisterW(GraphBuilder, UAVName);

	BoidsRDGStateData.ExecutePass[1] = UST_ComputeFunctionLibrary::AddCopyBufferPass(GraphBuilder, FlattenBoidsPingPongBuffer.WriteScopedRef, BoidsPingPongBuffer.WriteScopedRef);
	GraphBuilder.AddPassDependency(BoidsRDGStateData.ExecutePass[0], BoidsRDGStateData.ExecutePass[1]);

	GraphBuilder.QueueBufferExtraction(FlattenBoidsPingPongBuffer.WriteScopedRef, &FlattenBoidsPingPongBuffer.ReadPooled);
}