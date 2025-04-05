// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RenderGraphResources.h"
#include "ComputeFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class COMPUTECORE_API UComputeFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	static FRDGBufferRef CreateStructuredBuffer(
		FRDGBuilder& GraphBuilder,
		const TCHAR* Name,
		uint32 BytesPerElement, 
		uint32 NumElements,
		bool bReadOnly = false);

	static void RegisterSRV(
		FRDGBuilder& GraphBuilder,
		TRefCountPtr<FRDGPooledBuffer> buffer,
		FString name,
		FRDGBufferRef& outRDGRef,
		FRDGBufferSRVRef& outSRVRef,
		ERDGBufferFlags flags = ERDGBufferFlags::None);

	static void RegisterUAV(
		FRDGBuilder& GraphBuilder,
		TRefCountPtr<FRDGPooledBuffer> buffer,
		FString name,
		FRDGBufferRef& outRDGRef,
		FRDGBufferUAVRef& outUAVRef,
		ERDGBufferFlags flags = ERDGBufferFlags::None);

	static FRDGPassRef AddCopyBufferPass(FRDGBuilder& GraphBuilder, FRDGBufferRef DstBuffer, uint64 DstOffset, FRDGBufferRef SrcBuffer, uint64 SrcOffset, uint64 NumBytes);
	static FRDGPassRef AddCopyBufferPass(FRDGBuilder& GraphBuilder, FRDGBufferRef DstBuffer, FRDGBufferRef SrcBuffer);
};
