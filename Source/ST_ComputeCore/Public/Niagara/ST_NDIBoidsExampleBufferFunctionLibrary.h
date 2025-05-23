// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RenderGraphBuilder.h"
#include "GlobalShader.h"
#include "RenderGraphResources.h"
#include "ST_NDIBoidsExampleBufferFunctionLibrary.generated.h"

class UNiagaraComponent;

/**
 * 
 */
UCLASS()
class ST_COMPUTECORE_API UST_NDIBoidsExampleBufferFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	static void SetNiagaraBoidsExampleBuffer(UNiagaraComponent* NiagaraSystem, FName OverrideName, int32 numBoids, TRefCountPtr<FRDGPooledBuffer> InBuffer);
	
};