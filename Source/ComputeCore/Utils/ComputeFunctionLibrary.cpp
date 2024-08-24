// Fill out your copyright notice in the Description page of Project Settings.
#include "Utils/ComputeFunctionLibrary.h"
#include "RenderGraphUtils.h"
#include "RenderGraphBuilder.h"
#include "GlobalShader.h"

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