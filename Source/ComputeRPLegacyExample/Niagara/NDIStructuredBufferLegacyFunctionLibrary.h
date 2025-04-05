// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NDIStructuredBufferLegacyFunctionLibrary.generated.h"

class UNiagaraComponent;

/**
 * 
 */
UCLASS()
class COMPUTERPLEGACYEXAMPLE_API UNDIStructuredBufferLegacyFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	static void SetNiagaraStructuredBufferLegacy(UNiagaraComponent* NiagaraSystem, FName OverrideName, int32 numBoids, FShaderResourceViewRHIRef readRef);
	
};