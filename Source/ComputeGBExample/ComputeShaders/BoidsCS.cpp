#include "BoidsCS.h"
#include "RenderGraphUtils.h"

#define BoidsExample_ThreadsPerGroup 512

bool FBoidsGBInitExampleCS::ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
{
	// This example shader uses wave operations, so it requires SM6.
	return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM6);
}

void FBoidsGBInitExampleCS::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);

	OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_X"), BoidsExample_ThreadsPerGroup);
	OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_Y"), 1);
	OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_Z"), 1);
}

bool FBoidsGBUpdateExampleCS::ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
{
	// This example shader uses wave operations, so it requires SM6.
	return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM6);
}

void FBoidsGBUpdateExampleCS::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);

	OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_X"), BoidsExample_ThreadsPerGroup);
	OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_Y"), 1);
	OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_Z"), 1);
}

/**********************************************************************************************/
/* These functions schedule our Compute Shader work from the CPU!							  */
/**********************************************************************************************/

void FComputeShader_Boids::InitBoidsExample_RenderThread(FRDGBuilder& GraphBuilder, const TCHAR* OwnerName, const TArray<FBoidItem>& BoidsArray, const FBoidConstantParameters& BoidConstantParameters, const FBoidVariableParameters& BoidVariableParameters, FBoidsRenderGraphPasses& BoidsRenderGraphPasses, FPingPongBuffer& BoidsPingPongBuffer)
{
	const TCHAR* ReadBufferName = *(FString(OwnerName) + TEXT("BoidsInBuffer"));
	const TCHAR* WriteBufferName = *(FString(OwnerName) + TEXT("BoidsOutBuffer"));
	const TCHAR* SRVName = *(FString(OwnerName) + TEXT("BoidsIn_StructuredBuffer"));
	const TCHAR* UAVName = *(FString(OwnerName) + TEXT("BoidsOut_StructuredBuffer"));

	// First we create an RDG buffer with the appropriate size, and then instruct the graph to upload our CPU data to it.
	FRDGBufferDesc ReadDesc = FRDGBufferDesc::CreateStructuredDesc(BoidsArray.GetTypeSize(), BoidsArray.Num());
	FRDGBufferRef ReadBuffer = GraphBuilder.CreateBuffer(ReadDesc, ReadBufferName);
	GraphBuilder.QueueBufferUpload(ReadBuffer, BoidsArray.GetData(), BoidsArray.GetTypeSize() * BoidsArray.Num(), ERDGInitialDataFlags::None);

	FRDGBufferDesc WriteDesc = FRDGBufferDesc::CreateStructuredDesc(BoidsArray.GetTypeSize(), BoidsArray.Num());
	FRDGBufferRef WriteBuffer = GraphBuilder.CreateBuffer(WriteDesc, WriteBufferName);
	GraphBuilder.QueueBufferUpload(WriteBuffer, BoidsArray.GetData(), BoidsArray.GetTypeSize() * BoidsArray.Num(), ERDGInitialDataFlags::None);
	
	BoidsPingPongBuffer.ReadPooled = GraphBuilder.ConvertToExternalBuffer(ReadBuffer);
	BoidsPingPongBuffer.WritePooled = GraphBuilder.ConvertToExternalBuffer(WriteBuffer);

	BoidsPingPongBuffer.RegisterRW(GraphBuilder, SRVName, UAVName);

	QUICK_SCOPE_CYCLE_COUNTER(STAT_ComputeGBExample_Init); // Used to gather CPU profiling data for Unreal Insights.
	SCOPED_DRAW_EVENT(GraphBuilder.RHICmdList, ComputeGBExample_Init); // Used to profile GPU activity and add metadata to be consumed by for example RenderDoc

	FBoidsGBInitExampleCS::FParameters* PassParameters = GraphBuilder.AllocParameters<FBoidsGBInitExampleCS::FParameters>();
	PassParameters->numBoids = BoidConstantParameters.numBoids;
	PassParameters->maxSpeed = BoidVariableParameters.maxSpeed;
	PassParameters->boidsOut = BoidsPingPongBuffer.WriteScopedUAV;
	
	PassParameters->boundsInverseMatrix = BoidVariableParameters.inverseTransformMatrix;
	PassParameters->boundsMatrix = BoidVariableParameters.transformMatrix;
	PassParameters->boundsRadius = BoidVariableParameters.boundsRadius;

	PassParameters->randSeed = FMath::Rand() % (INT32_MAX + 1);

	TShaderMapRef<FBoidsGBInitExampleCS> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	FIntVector GroupCounts = FIntVector(FMath::DivideAndRoundUp(BoidConstantParameters.numBoids, BoidsExample_ThreadsPerGroup), 1, 1);

	BoidsRenderGraphPasses.init = true;
	BoidsRenderGraphPasses.InitPass = FComputeShaderUtils::AddPass(GraphBuilder,
		RDG_EVENT_NAME("InitBoidsExample"),
		ERDGPassFlags::Compute | ERDGPassFlags::NeverCull,
		ComputeShader,
		PassParameters,
		GroupCounts);

	BoidsPingPongBuffer.PingPong(GraphBuilder);
}

void FComputeShader_Boids::GetBoidsExample_RenderThread(FRDGBuilder& GraphBuilder, FBoidsGPUDispatches& BoidsGPUDispatches)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_ComputeGBExample_Get);
	// Here we look through our previous summation requests and finish any that might be ready.
	for (auto& [RequestId, BoidsGPURequest] : BoidsGPUDispatches.BoidsGPURequests)
	{
		if (!BoidsGPURequest.bReadbackComplete && BoidsGPURequest.Readback.IsReady())
		{
			FBoidItem* Buffer = (FBoidItem*)BoidsGPURequest.Readback.Lock(BoidsGPURequest.NrWorkGroups * (sizeof(FBoidItem)));

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

void FComputeShader_Boids::DispatchBoidsExample_RenderThread(FRDGBuilder& GraphBuilder, const TCHAR* OwnerName, const FBoidConstantParameters& BoidConstantParameters, const FBoidVariableParameters& BoidVariableParameters, FBoidsRenderGraphPasses& BoidsRenderGraphPasses, FPingPongBuffer& BoidsPingPongBuffer, float DeltaTime)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_ComputeGBExample_Dispatch); // Used to gather CPU profiling data for Unreal Insights.
	SCOPED_DRAW_EVENT(GraphBuilder.RHICmdList, ComputeGBExample_Compute); // Used to profile GPU activity and add metadata to be consumed by for example RenderDoc


	// This shader shows how you can use a compute shader to write to a texture.
	// Here we send our inputs using parameters as part of the shader parameter struct. This is an efficient way of sending in simple constants for a shader, but does not work well if you need to send larger amounts of data.
	if (!BoidsRenderGraphPasses.InitPass)
	{
		const TCHAR* SRVName = *(FString(OwnerName) + TEXT("BoidsIn_StructuredBuffer"));
		const TCHAR* UAVName = *(FString(OwnerName) + TEXT("BoidsOut_StructuredBuffer"));
		BoidsPingPongBuffer.RegisterRW(GraphBuilder, SRVName, UAVName);
	}

	FBoidsGBUpdateExampleCS::FParameters* PassParameters = GraphBuilder.AllocParameters<FBoidsGBUpdateExampleCS::FParameters>();
	PassParameters->numBoids = BoidConstantParameters.numBoids;
	PassParameters->deltaTime = DeltaTime * BoidVariableParameters.simulationSpeed;
	PassParameters->boidsIn = BoidsPingPongBuffer.ReadScopedSRV;
	PassParameters->boidsOut = BoidsPingPongBuffer.WriteScopedUAV;

	PassParameters->boundsInverseMatrix = BoidVariableParameters.inverseTransformMatrix;
	PassParameters->boundsMatrix = BoidVariableParameters.transformMatrix;
	PassParameters->boundsRadius = BoidVariableParameters.boundsRadius;

	PassParameters->minSpeed = BoidVariableParameters.minSpeed();
	PassParameters->maxSpeed = BoidVariableParameters.maxSpeed;
	PassParameters->turnSpeed = BoidVariableParameters.turnSpeed();
	PassParameters->minDistance = BoidVariableParameters.minDistance;
	PassParameters->minDistanceSq = BoidVariableParameters.minDistanceSq();
	PassParameters->cohesionFactor = BoidVariableParameters.cohesionFactor;
	PassParameters->separationFactor = BoidVariableParameters.separationFactor;
	PassParameters->alignmentFactor = BoidVariableParameters.alignmentFactor;

	TShaderMapRef<FBoidsGBUpdateExampleCS> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	FIntVector GroupCounts = FIntVector(FMath::DivideAndRoundUp(BoidConstantParameters.numBoids, BoidsExample_ThreadsPerGroup), 1, 1);
	BoidsRenderGraphPasses.UpdatePass = FComputeShaderUtils::AddPass(GraphBuilder,
		RDG_EVENT_NAME("BoidsUpdateExample"),
		ERDGPassFlags::Compute | ERDGPassFlags::NeverCull,
		ComputeShader,
		PassParameters,
		GroupCounts);

	if (BoidsRenderGraphPasses.InitPass)
	{
		GraphBuilder.AddPassDependency(BoidsRenderGraphPasses.InitPass, BoidsRenderGraphPasses.UpdatePass);
	}

	//AddCopyBufferPass(GraphBuilder, BoidsPingPongBuffer.WriteScopedRef, BoidsPingPongBuffer.ReadScopedRef);
	GraphBuilder.QueueBufferExtraction(BoidsPingPongBuffer.WriteScopedRef, &BoidsPingPongBuffer.ReadPooled);
	//GraphBuilder.Execute();
}

void FComputeShader_Boids::DispatchBoidsReadbackExample_RenderThread(FRDGBuilder& GraphBuilder, const TCHAR* OwnerName, const FBoidConstantParameters& BoidConstantParameters, const FBoidVariableParameters& BoidVariableParameters, FBoidsRenderGraphPasses& BoidsRenderGraphPasses, FPingPongBuffer& BoidsPingPongBuffer, float DeltaTime, FBoidsGPUReadback& BoidsGPURequest)
{
	DispatchBoidsExample_RenderThread(GraphBuilder, OwnerName, BoidConstantParameters, BoidVariableParameters, BoidsRenderGraphPasses, BoidsPingPongBuffer, DeltaTime);

	BoidsGPURequest.NrWorkGroups = FMath::DivideAndRoundUp(BoidConstantParameters.numBoids, BoidsExample_ThreadsPerGroup);
	BoidsGPURequest.BufferLength = BoidConstantParameters.numBoids;

	FRHIGPUBufferReadback* Readback = &BoidsGPURequest.Readback;
	FRDGBufferRef writeRef = BoidsPingPongBuffer.WriteScopedRef;

	AddReadbackBufferPass(GraphBuilder, RDG_EVENT_NAME("ReadbackReduceSum_RenderThread"), writeRef,
		[Readback, writeRef](FRHICommandList& RHICmdList)
		{
			Readback->EnqueueCopy(RHICmdList, writeRef->GetRHI());
		});
}