// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RenderGraphBuilder.h"
#include "GlobalShader.h"
#include "RenderGraphResources.h"
#include "NDIStructuredBufferFunctionLibrary.generated.h"

class UNiagaraComponent;

/**
 * 
 */
UCLASS()
class COMPUTECORE_API UNDIStructuredBufferFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	static void SetNiagaraStructuredBuffer(UNiagaraComponent* NiagaraSystem, FName OverrideName, int32 numBoids, TRefCountPtr<FRDGPooledBuffer> InBuffer);
	
};