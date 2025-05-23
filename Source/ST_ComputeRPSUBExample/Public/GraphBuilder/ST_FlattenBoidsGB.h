// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#pragma once

#include "Data/ST_BoidsLib.h"
#include "Utils/ST_PingPongBuffer.h"

class FST_GraphBullder_FlattenBoids
{

public:

	static void InitFlattenBoidsExample_RenderThread(FRDGBuilder& GraphBuilder,
		const TCHAR* OwnerName,
		const TArray<FST_BoidItem>& BoidsArray,
		FST_BoidsRDGStateData& BoidsRDGStateData,
		FST_PingPongBuffer& FlattenBoidsPingPongBuffer,
		FST_PingPongBuffer& BoidsPingPongBuffer);

	static void ExecuteFlattenBoidsExample_RenderThread(FRDGBuilder& GraphBuilder,
		const TCHAR* OwnerName,
		const FST_BoidCurrentParameters& BoidCurrentParameters,
		FST_BoidsRDGStateData& BoidsRDGStateData,
		FST_PingPongBuffer& FlattenBoidsPingPongBuffer,
		FST_PingPongBuffer& BoidsPingPongBuffer,
		float DeltaTime);
};