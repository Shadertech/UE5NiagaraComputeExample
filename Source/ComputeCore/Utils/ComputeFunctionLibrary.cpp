// Fill out your copyright notice in the Description page of Project Settings.
#include "Utils/ComputeFunctionLibrary.h"
#include "RenderGraphUtils.h"
#include "RenderGraphBuilder.h"
#include "GlobalShader.h"

FRDGBufferRef UComputeFunctionLibrary::CreateStructuredBuffer(
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

FRDGBufferRef UComputeFunctionLibrary::CreateRegisteredStructuredBuffer(
	FRDGBuilder& GraphBuilder,
	const TCHAR* Name,
	uint32 BytesPerElement,
	uint32 NumElements,
	bool bReadOnly,
	ERDGBufferFlags flags)
{
	FRHICommandListBase& RHICmdList = GraphBuilder.RHICmdList;

	FRHIResourceCreateInfo CreateInfo(Name);
	FRDGBufferDesc BufferDesc = FRDGBufferDesc::CreateStructuredDesc(BytesPerElement, NumElements);

	if (bReadOnly)
	{
		// Remove the UAV flag, as default resources are supposed to be read-only.
		EnumRemoveFlags(BufferDesc.Usage, EBufferUsageFlags::UnorderedAccess);
	}

	const uint32 BufferSize = BytesPerElement * NumElements;

	TRefCountPtr<FRHIBuffer> RHIBuffer = RHICmdList.CreateStructuredBuffer(BytesPerElement, BufferSize, BufferDesc.Usage, CreateInfo);
	TRefCountPtr<FRDGPooledBuffer> DummyPooledBuffer = new FRDGPooledBuffer(RHICmdList, RHIBuffer, BufferDesc, NumElements, Name);

	uint8* DestPtr = static_cast<uint8*>(RHICmdList.LockBuffer(RHIBuffer, 0, BufferSize, RLM_WriteOnly));
	FMemory::Memzero(DestPtr, BufferSize);
	RHICmdList.UnlockBuffer(RHIBuffer);

	return GraphBuilder.RegisterExternalBuffer(DummyPooledBuffer, flags);
}

void UComputeFunctionLibrary::RegisterSRV(
	FRDGBuilder& GraphBuilder,
	TRefCountPtr<FRDGPooledBuffer> buffer,
	FString name,
	FRDGBufferRef& outRDGRef,
	FRDGBufferSRVRef& outSRVRef,
	ERDGBufferFlags flags)
{
	outRDGRef = GraphBuilder.RegisterExternalBuffer(buffer, *name, flags);//, ERDGBufferFlags::MultiFrame);
	outSRVRef = GraphBuilder.CreateSRV(outRDGRef);
}

void UComputeFunctionLibrary::RegisterUAV(
	FRDGBuilder& GraphBuilder,
	TRefCountPtr<FRDGPooledBuffer> buffer,
	FString name,
	FRDGBufferRef& outRDGRef,
	FRDGBufferUAVRef& outUAVRef,
	ERDGBufferFlags flags)
{
	outRDGRef = GraphBuilder.RegisterExternalBuffer(buffer, *name, flags);//, ERDGBufferFlags::MultiFrame);
	outUAVRef = GraphBuilder.CreateUAV(outRDGRef);
}

bool UComputeFunctionLibrary::IsPlaying(const UObject* WorldContextObject)
{
#if WITH_EDITOR
	UWorld* World = WorldContextObject->GetWorld();
	if (World && !World->IsGameWorld())
	{
		return false;
	}
	return true;
#else
	return true;
#endif
}

BEGIN_SHADER_PARAMETER_STRUCT(FCopyBufferParameters, )
	RDG_BUFFER_ACCESS(SrcBuffer, ERHIAccess::CopySrc)
	RDG_BUFFER_ACCESS(DstBuffer, ERHIAccess::CopyDest)
END_SHADER_PARAMETER_STRUCT()

FRDGPassRef UComputeFunctionLibrary::AddCopyBufferPass(FRDGBuilder& GraphBuilder, FRDGBufferRef DstBuffer, uint64 DstOffset, FRDGBufferRef SrcBuffer, uint64 SrcOffset, uint64 NumBytes)
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

FRDGPassRef UComputeFunctionLibrary::AddCopyBufferPass(FRDGBuilder& GraphBuilder, FRDGBufferRef DstBuffer, FRDGBufferRef SrcBuffer)
{
	check(SrcBuffer);
	check(DstBuffer);

	const uint64 NumBytes = SrcBuffer->Desc.NumElements * SrcBuffer->Desc.BytesPerElement;

	return UComputeFunctionLibrary::AddCopyBufferPass(GraphBuilder, DstBuffer, 0, SrcBuffer, 0, NumBytes);
}