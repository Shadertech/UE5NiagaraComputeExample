#pragma once

#include "Data/BoidsLib.h"
#include "Utils/PingPongBuffer.h"

class FGraphBullder_Boids
{

public:

	static void InitBoidsExample_RenderThread(FRDGBuilder& GraphBuilder,
		const TCHAR* OwnerName,
		const TArray<FBoidItem>& BoidsArray,
		const FBoidCurrentParameters& BoidCurrentParameters,
		FBoidsRenderGraphPasses& BoidsRenderGraphPasses,
		FPingPongBuffer& BoidsPingPongBuffer);
	
	static void GetBoidsExample_RenderThread(FRDGBuilder& GraphBuilder,
		FBoidsGPUDispatches& BoidsGPUDispatches);
	
	static void ExecuteBoidsExample_RenderThread(FRDGBuilder& GraphBuilder,
		const TCHAR* OwnerName,
		const FBoidCurrentParameters& BoidCurrentParameters,
		FBoidsRenderGraphPasses& BoidsRenderGraphPasses,
		FPingPongBuffer& BoidsPingPongBuffer,
		float DeltaTime);
	
	static void ExecuteBoidsReadbackExample_RenderThread(FRDGBuilder& GraphBuilder,
		const TCHAR* OwnerName,
		const FBoidCurrentParameters& BoidCurrentParameters,
		FBoidsRenderGraphPasses& BoidsRenderGraphPasses,
		FPingPongBuffer& BoidsPingPongBuffer,
		float DeltaTime,
		FBoidsGPUReadback& BoidsGPURequest);
};