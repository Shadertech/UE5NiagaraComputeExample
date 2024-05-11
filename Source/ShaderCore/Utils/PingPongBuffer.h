#pragma once
#include "RenderGraphUtils.h"
#include "RenderGraphBuilder.h"
#include "GlobalShader.h"

void RegisterSRV(
	FRDGBuilder& GraphBuilder,
	TRefCountPtr<FRDGPooledBuffer> buffer,
	FString name,
	FRDGBufferRef& outRDGRef,
	FRDGBufferSRVRef& outSRVRef)
{
	outRDGRef = GraphBuilder.RegisterExternalBuffer(buffer, *name);//, ERDGBufferFlags::MultiFrame);
	outSRVRef = GraphBuilder.CreateSRV(outRDGRef);
}

void RegisterUAV(
	FRDGBuilder& GraphBuilder,
	TRefCountPtr<FRDGPooledBuffer> buffer,
	FString name,
	FRDGBufferRef& outRDGRef,
	FRDGBufferUAVRef& outUAVRef)
{
	outRDGRef = GraphBuilder.RegisterExternalBuffer(buffer, *name);//, ERDGBufferFlags::MultiFrame);
	outUAVRef = GraphBuilder.CreateUAV(outRDGRef);
}

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
		RegisterSRV(GraphBuilder, ReadPooled, readName, ReadScopedRef, ReadScopedSRV);
		RegisterUAV(GraphBuilder, WritePooled, writeName, WriteScopedRef, WriteScopedUAV);
	}
};