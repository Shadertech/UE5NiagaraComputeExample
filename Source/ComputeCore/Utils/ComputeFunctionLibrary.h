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

	UFUNCTION(BlueprintPure, Category = "ComputeExample", meta = (BlueprintThreadSafe, WorldContext = "WorldContextObject"))
	static bool IsPlaying(const UObject* WorldContextObject);
};
