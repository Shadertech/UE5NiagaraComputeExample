// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#pragma once

#include "Data/ST_BoidsLib.h"
#include "Utils/ST_PingPongBuffer.h"

class ST_COMPUTERPEXAMPLE_API FST_GraphBullder_Boids
{

public:

	static void InitBoidsExample_RenderThread(FRDGBuilder& GraphBuilder,
		const TCHAR* OwnerName,
		const TArray<FST_BoidItem>& BoidsArray,
		const FST_BoidCurrentParameters& BoidCurrentParameters,
		FST_BoidsRDGStateData& BoidsRDGStateData,
		FST_PingPongBuffer& BoidsPingPongBuffer);
	
	static void GetBoidsExample_RenderThread(FRDGBuilder& GraphBuilder,
		FST_BoidsGPUDispatches& BoidsGPUDispatches);
	
	static void ExecuteBoidsExample_RenderThread(FRDGBuilder& GraphBuilder,
		const TCHAR* OwnerName,
		const FST_BoidCurrentParameters& BoidCurrentParameters,
		FST_BoidsRDGStateData& BoidsRDGStateData,
		FST_PingPongBuffer& BoidsPingPongBuffer,
		float DeltaTime);
	
	static void ExecuteBoidsReadbackExample_RenderThread(FRDGBuilder& GraphBuilder,
		const TCHAR* OwnerName,
		const FST_BoidCurrentParameters& BoidCurrentParameters,
		FST_BoidsRDGStateData& BoidsRDGStateData,
		FST_PingPongBuffer& BoidsPingPongBuffer,
		float DeltaTime,
		FST_BoidsGPUReadback& BoidsGPURequest);
};