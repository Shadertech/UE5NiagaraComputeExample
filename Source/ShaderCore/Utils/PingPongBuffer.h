#pragma once
#include "RenderGraphUtils.h"
#include "RenderGraphBuilder.h"
#include "GlobalShader.h"
#include "ComputeFunctionLibrary.h"

struct FPingPongBuffer
{
    TRefCountPtr<FRDGPooledBuffer> ReadPooled;
    TRefCountPtr<FRDGPooledBuffer> WritePooled;
	FRDGBufferRef ReadScopedRef = nullptr;
	FRDGBufferRef WriteScopedRef = nullptr;
	FRDGBufferSRVRef ReadScopedSRV = nullptr;
	FRDGBufferUAVRef WriteScopedUAV = nullptr;

    FPingPongBuffer()
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
		std::swap(ReadScopedRef, WriteScopedRef);

		ReadScopedSRV = GraphBuilder.CreateSRV(ReadScopedRef);
		WriteScopedUAV = GraphBuilder.CreateUAV(WriteScopedRef);
	}

	void RegisterRW(FRDGBuilder& GraphBuilder, FString readName, FString writeName)
	{
		UComputeFunctionLibrary::RegisterSRV(GraphBuilder, ReadPooled, readName, ReadScopedRef, ReadScopedSRV);
		UComputeFunctionLibrary::RegisterUAV(GraphBuilder, WritePooled, writeName, WriteScopedRef, WriteScopedUAV);
	}
};