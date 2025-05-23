// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RHIFwd.h"
#include "ST_NDIBoidsExampleBufferLegacyFunctionLibrary.generated.h"

class UNiagaraComponent;

/**
 * 
 */
UCLASS()
class ST_COMPUTERPLEGACYEXAMPLE_API UST_NDIBoidsExampleBufferLegacyFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	static void SetNiagaraBoidsExampleBufferLegacy(UNiagaraComponent* NiagaraSystem, FName OverrideName, int32 numBoids, FShaderResourceViewRHIRef readRef);
	
};