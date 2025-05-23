// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#include "GraphBuilder/ST_BoidsGB.h"
#include "RenderGraphUtils.h"
#include "ComputeShaders/ST_BoidsRPCS.h"

#define BoidsExample_ThreadsPerGroup 512

void FST_GraphBullder_Boids::InitBoidsExample_RenderThread(FRDGBuilder& GraphBuilder, const TCHAR* OwnerName, const TArray<FST_BoidItem>& BoidsArray,  const FST_BoidCurrentParameters& BoidCurrentParameters, FST_BoidsRDGStateData& BoidsRDGStateData, FST_PingPongBuffer& BoidsPingPongBuffer)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_BoidsExample_Init);
	SCOPED_DRAW_EVENT(GraphBuilder.RHICmdList, BoidsExample_Init);

	const TCHAR* ReadBufferName = *(FString(OwnerName) + TEXT("BoidsInBuffer"));
	const TCHAR* WriteBufferName = *(FString(OwnerName) + TEXT("BoidsOutBuffer"));
	const TCHAR* SRVName = *(FString(OwnerName) + TEXT("BoidsIn_StructuredBuffer"));
	const TCHAR* UAVName = *(FString(OwnerName) + TEXT("BoidsOut_StructuredBuffer"));

	// First we create an RDG buffer with the appropriate size, and then instruct the graph to upload our CPU data to it.
	FRDGBufferRef ReadBuffer = UST_ComputeFunctionLibrary::CreateStructuredBuffer(GraphBuilder, ReadBufferName, BoidsArray.GetTypeSize(), BoidsArray.Num());
	GraphBuilder.QueueBufferUpload(ReadBuffer, BoidsArray.GetData(), BoidsArray.GetTypeSize() * BoidsArray.Num(), ERDGInitialDataFlags::None);

	FRDGBufferRef WriteBuffer = UST_ComputeFunctionLibrary::CreateStructuredBuffer(GraphBuilder, WriteBufferName, BoidsArray.GetTypeSize(), BoidsArray.Num());
	GraphBuilder.QueueBufferUpload(WriteBuffer, BoidsArray.GetData(), BoidsArray.GetTypeSize() * BoidsArray.Num(), ERDGInitialDataFlags::None);
	
	BoidsPingPongBuffer.ReadPooled = GraphBuilder.ConvertToExternalBuffer(ReadBuffer);
	BoidsPingPongBuffer.WritePooled = GraphBuilder.ConvertToExternalBuffer(WriteBuffer);

	BoidsPingPongBuffer.RegisterRW(GraphBuilder, SRVName, UAVName);

	FST_BoidsRPInitExampleCS::FParameters* PassParameters = GraphBuilder.AllocParameters<FST_BoidsRPInitExampleCS::FParameters>();
	PassParameters->numBoids = BoidCurrentParameters.ConstantParameters.numBoids;
	PassParameters->maxSpeed = BoidCurrentParameters.DynamicParameters.maxSpeed;
	PassParameters->boidsOut = BoidsPingPongBuffer.WriteScopedUAV;
	
	PassParameters->boundsInverseMatrix = BoidCurrentParameters.inverseTransformMatrix;
	PassParameters->boundsMatrix = BoidCurrentParameters.transformMatrix;
	PassParameters->boundsRadius = BoidCurrentParameters.boundsRadius;

	PassParameters->randSeed = FMath::Rand() % (INT32_MAX + 1);

	TShaderMapRef<FST_BoidsRPInitExampleCS> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	FIntVector GroupCounts = FIntVector(FMath::DivideAndRoundUp(BoidCurrentParameters.ConstantParameters.numBoids, BoidsExample_ThreadsPerGroup), 1, 1);

	BoidsRDGStateData.InitPass[0] = FComputeShaderUtils::AddPass(GraphBuilder,
		RDG_EVENT_NAME("RP_InitBoidsExample"),
		ERDGPassFlags::Compute | ERDGPassFlags::NeverCull,
		ComputeShader,
		PassParameters,
		GroupCounts);

	GraphBuilder.QueueBufferExtraction(BoidsPingPongBuffer.WriteScopedRef, &BoidsPingPongBuffer.ReadPooled);
}

void FST_GraphBullder_Boids::GetBoidsExample_RenderThread(FRDGBuilder& GraphBuilder, FST_BoidsGPUDispatches& BoidsGPUDispatches)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_BoidsExample_Get);
	// Here we look through our previous summation requests and finish any that might be ready.
	for (auto& [RequestId, BoidsGPURequest] : BoidsGPUDispatches.BoidsGPURequests)
	{
		if (!BoidsGPURequest.bReadbackComplete && BoidsGPURequest.Readback.IsReady())
		{
			FST_BoidItem* Buffer = (FST_BoidItem*)BoidsGPURequest.Readback.Lock(BoidsGPURequest.NrWorkGroups * (sizeof(FST_BoidItem)));

			if (!Buffer)
			{
				continue;
			}

			BoidsGPURequest.Result.Append(Buffer, BoidsGPURequest.BufferLength);
			//ToDo: readback multiple groups
			/*for (int32 GroupDataIdx = 0; GroupDataIdx < BoidsGPURequest.NrWorkGroups; GroupDataIdx++)
			{
				BoidsGPURequest.Result += Buffer[GroupDataIdx]; ThreadsPerGroup
			}*/

			BoidsGPURequest.Readback.Unlock();
			BoidsGPURequest.bReadbackComplete = true;
		}
	}
}

void FST_GraphBullder_Boids::ExecuteBoidsExample_RenderThread(FRDGBuilder& GraphBuilder, const TCHAR* OwnerName,  const FST_BoidCurrentParameters& BoidCurrentParameters, FST_BoidsRDGStateData& BoidsRDGStateData, FST_PingPongBuffer& BoidsPingPongBuffer, float DeltaTime)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_BoidsExample_Execute);
	SCOPED_DRAW_EVENT(GraphBuilder.RHICmdList, BoidsExample_Execute);

	const TCHAR* SRVName = *(FString(OwnerName) + TEXT("BoidsIn_StructuredBuffer"));
	const TCHAR* UAVName = *(FString(OwnerName) + TEXT("BoidsOut_StructuredBuffer"));
	BoidsPingPongBuffer.RegisterRW(GraphBuilder, SRVName, UAVName);

	FST_BoidsRPUpdateExampleCS::FParameters* PassParameters = GraphBuilder.AllocParameters<FST_BoidsRPUpdateExampleCS::FParameters>();
	PassParameters->numBoids = BoidCurrentParameters.ConstantParameters.numBoids;
	PassParameters->deltaTime = DeltaTime * BoidCurrentParameters.DynamicParameters.simulationSpeed;
	PassParameters->boidsIn = BoidsPingPongBuffer.ReadScopedSRV;
	PassParameters->boidsOut = BoidsPingPongBuffer.WriteScopedUAV;

	PassParameters->boundsInverseMatrix = BoidCurrentParameters.inverseTransformMatrix;
	PassParameters->boundsMatrix = BoidCurrentParameters.transformMatrix;
	PassParameters->boundsRadius = BoidCurrentParameters.boundsRadius;

	PassParameters->minSpeed = BoidCurrentParameters.DynamicParameters.minSpeed();
	PassParameters->maxSpeed = BoidCurrentParameters.DynamicParameters.maxSpeed;
	PassParameters->turnSpeed = BoidCurrentParameters.DynamicParameters.turnSpeed();
	PassParameters->minDistance = BoidCurrentParameters.DynamicParameters.minDistance;
	PassParameters->minDistanceSq = BoidCurrentParameters.DynamicParameters.minDistanceSq();
	PassParameters->cohesionFactor = BoidCurrentParameters.DynamicParameters.cohesionFactor;
	PassParameters->separationFactor = BoidCurrentParameters.DynamicParameters.separationFactor;
	PassParameters->alignmentFactor = BoidCurrentParameters.DynamicParameters.alignmentFactor;

	TShaderMapRef<FST_BoidsRPUpdateExampleCS> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	FIntVector GroupCounts = FIntVector(FMath::DivideAndRoundUp(BoidCurrentParameters.ConstantParameters.numBoids, BoidsExample_ThreadsPerGroup), 1, 1);
	BoidsRDGStateData.ExecutePass[0] = FComputeShaderUtils::AddPass(GraphBuilder,
		RDG_EVENT_NAME("RP_BoidsUpdateExample"),
		ERDGPassFlags::Compute | ERDGPassFlags::NeverCull,
		ComputeShader,
		PassParameters,
		GroupCounts);

	//AddCopyBufferPass(GraphBuilder, BoidsPingPongBuffer.WriteScopedRef, BoidsPingPongBuffer.ReadScopedRef);
	GraphBuilder.QueueBufferExtraction(BoidsPingPongBuffer.WriteScopedRef, &BoidsPingPongBuffer.ReadPooled);
}

void FST_GraphBullder_Boids::ExecuteBoidsReadbackExample_RenderThread(FRDGBuilder& GraphBuilder, const TCHAR* OwnerName,  const FST_BoidCurrentParameters& BoidCurrentParameters, FST_BoidsRDGStateData& BoidsRDGStateData, FST_PingPongBuffer& BoidsPingPongBuffer, float DeltaTime, FST_BoidsGPUReadback& BoidsGPURequest)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_BoidsExample_Readback);

	ExecuteBoidsExample_RenderThread(GraphBuilder, OwnerName, BoidCurrentParameters, BoidsRDGStateData, BoidsPingPongBuffer, DeltaTime);

	BoidsGPURequest.NrWorkGroups = FMath::DivideAndRoundUp(BoidCurrentParameters.ConstantParameters.numBoids, BoidsExample_ThreadsPerGroup);
	BoidsGPURequest.BufferLength = BoidCurrentParameters.ConstantParameters.numBoids;

	FRHIGPUBufferReadback* Readback = &BoidsGPURequest.Readback;
	FRDGBufferRef writeRef = BoidsPingPongBuffer.WriteScopedRef;

	AddReadbackBufferPass(GraphBuilder, RDG_EVENT_NAME("ReadbackReduceSum_RenderThread"), writeRef,
		[Readback, writeRef](FRHICommandList& RHICmdList)
		{
			Readback->EnqueueCopy(RHICmdList, writeRef->GetRHI());
		});
}