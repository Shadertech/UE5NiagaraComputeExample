// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NDIStructuredBufferFunctionLibrary.generated.h"

class UNiagaraComponent;

/**
 * 
 */
UCLASS(MinimalAPI)
class UNDIStructuredBufferFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	static COMPUTECORE_API void SetNiagaraStructuredBuffer(UNiagaraComponent* NiagaraSystem, FName OverrideName, int32 numBoids, TRefCountPtr<FRDGPooledBuffer> InBuffer);
	
};