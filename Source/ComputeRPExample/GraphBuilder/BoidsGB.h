// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#pragma once

#include "Data/BoidsLib.h"
#include "Utils/PingPongBuffer.h"

class COMPUTERPEXAMPLE_API FGraphBullder_Boids
{

public:

	static void InitBoidsExample_RenderThread(FRDGBuilder& GraphBuilder,
		const TCHAR* OwnerName,
		const TArray<FBoidItem>& BoidsArray,
		const FBoidCurrentParameters& BoidCurrentParameters,
		FBoidsRDGStateData& BoidsRDGStateData,
		FPingPongBuffer& BoidsPingPongBuffer);
	
	static void GetBoidsExample_RenderThread(FRDGBuilder& GraphBuilder,
		FBoidsGPUDispatches& BoidsGPUDispatches);
	
	static void ExecuteBoidsExample_RenderThread(FRDGBuilder& GraphBuilder,
		const TCHAR* OwnerName,
		const FBoidCurrentParameters& BoidCurrentParameters,
		FBoidsRDGStateData& BoidsRDGStateData,
		FPingPongBuffer& BoidsPingPongBuffer,
		float DeltaTime);
	
	static void ExecuteBoidsReadbackExample_RenderThread(FRDGBuilder& GraphBuilder,
		const TCHAR* OwnerName,
		const FBoidCurrentParameters& BoidCurrentParameters,
		FBoidsRDGStateData& BoidsRDGStateData,
		FPingPongBuffer& BoidsPingPongBuffer,
		float DeltaTime,
		FBoidsGPUReadback& BoidsGPURequest);
};