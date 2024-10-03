#pragma once

#include "Data/BoidsLib.h"
#include "Utils/PingPongBuffer.h"

class FGraphBullder_FlattenBoids
{

public:

	static void InitFlattenBoidsExample_RenderThread(FRDGBuilder& GraphBuilder,
		const TCHAR* OwnerName,
		const TArray<FBoidItem>& BoidsArray,
		FBoidsRenderGraphPasses& FlattenBoidsRenderGraphPasses,
		FPingPongBuffer& FlattenBoidsPingPongBuffer,
		FPingPongBuffer& BoidsPingPongBuffer);

	static void ExecuteFlattenBoidsExample_RenderThread(FRDGBuilder& GraphBuilder,
		const TCHAR* OwnerName,
		const FBoidCurrentParameters& BoidCurrentParameters,
		FBoidsRenderGraphPasses& FlattenBoidsRenderGraphPasses,
		FPingPongBuffer& FlattenBoidsPingPongBuffer,
		FPingPongBuffer& BoidsPingPongBuffer,
		float DeltaTime);
};