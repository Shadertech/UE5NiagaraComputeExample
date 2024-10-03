// Fill out your copyright notice in the Description page of Project Settings.
#include "Utils/ComputeFunctionLibrary.h"
#include "RenderGraphUtils.h"
#include "RenderGraphBuilder.h"
#include "GlobalShader.h"

FRDGBufferRef UComputeFunctionLibrary::CreateStructuredBuffer(
	FRDGBuilder& GraphBuilder,
	const TCHAR* Name,
	uint32 BytesPerElement,
	uint32 NumElements)
{
	FRDGBufferDesc Desc = FRDGBufferDesc::CreateStructuredDesc(BytesPerElement, NumElements);
	return GraphBuilder.CreateBuffer(Desc, Name);
}

void UComputeFunctionLibrary::RegisterSRV(
	FRDGBuilder& GraphBuilder,
	TRefCountPtr<FRDGPooledBuffer> buffer,
	FString name,
	FRDGBufferRef& outRDGRef,
	FRDGBufferSRVRef& outSRVRef)
{
	outRDGRef = GraphBuilder.RegisterExternalBuffer(buffer, *name);//, ERDGBufferFlags::MultiFrame);
	outSRVRef = GraphBuilder.CreateSRV(outRDGRef);
}

void UComputeFunctionLibrary::RegisterUAV(
	FRDGBuilder& GraphBuilder,
	TRefCountPtr<FRDGPooledBuffer> buffer,
	FString name,
	FRDGBufferRef& outRDGRef,
	FRDGBufferUAVRef& outUAVRef)
{
	outRDGRef = GraphBuilder.RegisterExternalBuffer(buffer, *name);//, ERDGBufferFlags::MultiFrame);
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