// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#pragma once
#include "RenderGraphUtils.h"
#include "RenderGraphBuilder.h"
#include "GlobalShader.h"
#include "ST_ComputeFunctionLibrary.h"

struct FST_PingPongBuffer
{
    TRefCountPtr<FRDGPooledBuffer> ReadPooled = nullptr;
    TRefCountPtr<FRDGPooledBuffer> WritePooled = nullptr;
	FRDGBufferRef ReadScopedRef = nullptr;
	FRDGBufferRef WriteScopedRef = nullptr;
	FRDGBufferSRVRef ReadScopedSRV = nullptr;
	FRDGBufferUAVRef WriteScopedUAV = nullptr;

    FST_PingPongBuffer()
    {
        ReadPooled = nullptr;
        WritePooled = nullptr;
		ReadScopedRef = nullptr;
		WriteScopedRef = nullptr;
		ReadScopedSRV = nullptr;
		WriteScopedUAV = nullptr;
    };

	void PingPong(FRDGBuilder& GraphBuilder)
	{
		Swap(ReadScopedRef, WriteScopedRef);

		ReadScopedSRV = GraphBuilder.CreateSRV(ReadScopedRef);
		WriteScopedUAV = GraphBuilder.CreateUAV(WriteScopedRef);
	}

	void RegisterRW(FRDGBuilder& GraphBuilder, FString readName, FString writeName)
	{
		UST_ComputeFunctionLibrary::RegisterSRV(GraphBuilder, ReadPooled, readName, ReadScopedRef, ReadScopedSRV);
		UST_ComputeFunctionLibrary::RegisterUAV(GraphBuilder, WritePooled, writeName, WriteScopedRef, WriteScopedUAV);
	}

	void RegisterR(FRDGBuilder& GraphBuilder, FString readName)
	{
		UST_ComputeFunctionLibrary::RegisterSRV(GraphBuilder, ReadPooled, readName, ReadScopedRef, ReadScopedSRV);
	}

	void RegisterW(FRDGBuilder& GraphBuilder, FString writeName)
	{
		UST_ComputeFunctionLibrary::RegisterUAV(GraphBuilder, WritePooled, writeName, WriteScopedRef, WriteScopedUAV);
	}

	~FST_PingPongBuffer()
	{
		ReleaseData();
	}

	void ReleaseData()
	{
		if (ReadPooled.IsValid())
		{
			ReadPooled.SafeRelease();
		}

		if (WritePooled.IsValid())
		{
			WritePooled.SafeRelease();
		}

		ReadScopedRef = nullptr;
		WriteScopedRef = nullptr;
		ReadScopedSRV = nullptr;
		WriteScopedUAV = nullptr;
	}
};