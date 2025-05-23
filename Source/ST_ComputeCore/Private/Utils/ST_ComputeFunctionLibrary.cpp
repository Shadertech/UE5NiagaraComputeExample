// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#include "Utils/ST_ComputeFunctionLibrary.h"
#include "RenderGraphUtils.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphResources.h"
#include "GlobalShader.h"

FRDGBufferRef UST_ComputeFunctionLibrary::CreateStructuredBuffer(
	FRDGBuilder& GraphBuilder,
	const TCHAR* Name,
	uint32 BytesPerElement,
	uint32 NumElements,
	bool bReadOnly)
{
	FRDGBufferDesc Desc = FRDGBufferDesc::CreateStructuredDesc(BytesPerElement, NumElements);

	if(bReadOnly)
	{
		// Remove the UAV flag, as default resources are supposed to be read-only.
		EnumRemoveFlags(Desc.Usage, EBufferUsageFlags::UnorderedAccess);
	}

	return GraphBuilder.CreateBuffer(Desc, Name);
}

void UST_ComputeFunctionLibrary::RegisterSRV(
	FRDGBuilder& GraphBuilder,
	TRefCountPtr<FRDGPooledBuffer> buffer,
	FString name,
	FRDGBufferRef& outRDGRef,
	FRDGBufferSRVRef& outSRVRef,
	ERDGBufferFlags flags)
{
	outRDGRef = GraphBuilder.RegisterExternalBuffer(buffer, *name, flags);
	outSRVRef = GraphBuilder.CreateSRV(outRDGRef);
}

void UST_ComputeFunctionLibrary::RegisterUAV(
	FRDGBuilder& GraphBuilder,
	TRefCountPtr<FRDGPooledBuffer> buffer,
	FString name,
	FRDGBufferRef& outRDGRef,
	FRDGBufferUAVRef& outUAVRef,
	ERDGBufferFlags flags)
{
	outRDGRef = GraphBuilder.RegisterExternalBuffer(buffer, *name, flags);
	outUAVRef = GraphBuilder.CreateUAV(outRDGRef);
}

BEGIN_SHADER_PARAMETER_STRUCT(FCopyBufferParameters, )
	RDG_BUFFER_ACCESS(SrcBuffer, ERHIAccess::CopySrc)
	RDG_BUFFER_ACCESS(DstBuffer, ERHIAccess::CopyDest)
END_SHADER_PARAMETER_STRUCT()

FRDGPassRef UST_ComputeFunctionLibrary::AddCopyBufferPass(FRDGBuilder& GraphBuilder, FRDGBufferRef DstBuffer, uint64 DstOffset, FRDGBufferRef SrcBuffer, uint64 SrcOffset, uint64 NumBytes)
{
	check(SrcBuffer);
	check(DstBuffer);

	FCopyBufferParameters* Parameters = GraphBuilder.AllocParameters<FCopyBufferParameters>();
	Parameters->SrcBuffer = SrcBuffer;
	Parameters->DstBuffer = DstBuffer;

	FRDGPassRef CopyPass = GraphBuilder.AddPass(
		RDG_EVENT_NAME("CopyBuffer(%s Size=%ubytes)", SrcBuffer->Name, SrcBuffer->Desc.GetSize()),
		Parameters,
		ERDGPassFlags::Copy,
		[&Parameters, SrcBuffer, DstBuffer, SrcOffset, DstOffset, NumBytes](FRHICommandList& RHICmdList)
		{
			RHICmdList.CopyBufferRegion(DstBuffer->GetRHI(), DstOffset, SrcBuffer->GetRHI(), SrcOffset, NumBytes);
		});

	return CopyPass;
}

FRDGPassRef UST_ComputeFunctionLibrary::AddCopyBufferPass(FRDGBuilder& GraphBuilder, FRDGBufferRef DstBuffer, FRDGBufferRef SrcBuffer)
{
	check(SrcBuffer);
	check(DstBuffer);

	const uint64 NumBytes = SrcBuffer->Desc.NumElements * SrcBuffer->Desc.BytesPerElement;

	return UST_ComputeFunctionLibrary::AddCopyBufferPass(GraphBuilder, DstBuffer, 0, SrcBuffer, 0, NumBytes);
}