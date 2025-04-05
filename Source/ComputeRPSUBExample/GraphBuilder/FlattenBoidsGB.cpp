// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#include "GraphBuilder/FlattenBoidsGB.h"
#include "RenderGraphResources.h"
#include "RenderGraphBuilder.h"
#include "Utils/ComputeFunctionLibrary.h"
#include "RenderGraphUtils.h"
#include "ComputeShaders/BoidsRPSUBCS.h"

void FGraphBullder_FlattenBoids::InitFlattenBoidsExample_RenderThread(FRDGBuilder& GraphBuilder, const TCHAR* OwnerName, const TArray<FBoidItem>& BoidsArray, FBoidsRDGStateData& BoidsRDGStateData, FPingPongBuffer& FlattenBoidsPingPongBuffer, FPingPongBuffer& BoidsPingPongBuffer)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_FlattenBoidsExample_Init);
	SCOPED_DRAW_EVENT(GraphBuilder.RHICmdList, FlattenBoidsExample_Init);

	const TCHAR* WriteBufferName = *(FString(OwnerName) + TEXT("FlattenBoidsOutBuffer"));

	FRDGBufferRef WriteBuffer = UComputeFunctionLibrary::CreateStructuredBuffer(GraphBuilder, WriteBufferName, 4, BoidsArray.Num() * 12);
	FlattenBoidsPingPongBuffer.WritePooled = GraphBuilder.ConvertToExternalBuffer(WriteBuffer);
}

void FGraphBullder_FlattenBoids::ExecuteFlattenBoidsExample_RenderThread(FRDGBuilder& GraphBuilder, const TCHAR* OwnerName, const FBoidCurrentParameters& BoidCurrentParameters, FBoidsRDGStateData& BoidsRDGStateData, FPingPongBuffer& FlattenBoidsPingPongBuffer, FPingPongBuffer& BoidsPingPongBuffer, float DeltaTime)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_FlattenBoidsExample_Execute);
	SCOPED_DRAW_EVENT(GraphBuilder.RHICmdList, FlattenBoidsExample_Execute);

	const TCHAR* UAVName = *(FString(OwnerName) + TEXT("FlattenBoidsOut_StructuredBuffer"));
	FlattenBoidsPingPongBuffer.RegisterW(GraphBuilder, UAVName);

	BoidsRDGStateData.ExecutePass[1] = UComputeFunctionLibrary::AddCopyBufferPass(GraphBuilder, FlattenBoidsPingPongBuffer.WriteScopedRef, BoidsPingPongBuffer.WriteScopedRef);
	GraphBuilder.AddPassDependency(BoidsRDGStateData.ExecutePass[0], BoidsRDGStateData.ExecutePass[1]);

	GraphBuilder.QueueBufferExtraction(FlattenBoidsPingPongBuffer.WriteScopedRef, &FlattenBoidsPingPongBuffer.ReadPooled);
}