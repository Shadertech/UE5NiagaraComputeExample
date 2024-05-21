// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ComputeFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class SHADERCORE_API UComputeFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
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
};
