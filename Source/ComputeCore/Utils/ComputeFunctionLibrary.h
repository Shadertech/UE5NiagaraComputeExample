// Fill out your copyright notice in the Description page of Project Settings.

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
		uint32 NumElements);

	static void RegisterSRV(
		FRDGBuilder& GraphBuilder,
		TRefCountPtr<FRDGPooledBuffer> buffer,
		FString name,
		FRDGBufferRef& outRDGRef,
		FRDGBufferSRVRef& outSRVRef);

	static void RegisterUAV(
		FRDGBuilder& GraphBuilder,
		TRefCountPtr<FRDGPooledBuffer> buffer,
		FString name,
		FRDGBufferRef& outRDGRef,
		FRDGBufferUAVRef& outUAVRef);

	static FRDGPassRef AddCopyBufferPass(FRDGBuilder& GraphBuilder, FRDGBufferRef DstBuffer, uint64 DstOffset, FRDGBufferRef SrcBuffer, uint64 SrcOffset, uint64 NumBytes);
	static FRDGPassRef AddCopyBufferPass(FRDGBuilder& GraphBuilder, FRDGBufferRef DstBuffer, FRDGBufferRef SrcBuffer);

	UFUNCTION(BlueprintPure, Category = "ComputeExample", meta = (BlueprintThreadSafe, WorldContext = "WorldContextObject"))
	static bool IsPlaying(const UObject* WorldContextObject);
};
