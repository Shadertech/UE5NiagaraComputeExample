// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NDIStructuredBufferLegacyFunctionLibrary.generated.h"

class UNiagaraComponent;

/**
 * 
 */
UCLASS(MinimalAPI)
class UNDIStructuredBufferLegacyFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	static COMPUTERPLEGACYEXAMPLE_API void SetNiagaraStructuredBuffer(UNiagaraComponent* NiagaraSystem, FName OverrideName, int32 numBoids, FShaderResourceViewRHIRef readRef);
	
};