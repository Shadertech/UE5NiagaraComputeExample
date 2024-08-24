// Fill out your copyright notice in the Description page of Project Settings.


#include "Niagara/NDIStructuredBufferLegacyFunctionLibrary.h"

#include "RHIResources.h"
#include "Engine/World.h"
#include "NiagaraComponent.h"
#include "NiagaraSystemInstance.h"
#include "NiagaraSystemInstanceController.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraDataInterfaceStructuredBufferLegacy.h"

void UNDIStructuredBufferLegacyFunctionLibrary::SetNiagaraStructuredBuffer(UNiagaraComponent* NiagaraComponent, FName OverrideName, int32 numBoids, FShaderResourceViewRHIRef readRef)
{
	if (UNiagaraDataInterfaceStructuredBufferLegacy* SBDI = UNiagaraFunctionLibrary::GetDataInterface<UNiagaraDataInterfaceStructuredBufferLegacy>(NiagaraComponent, OverrideName))
	{
		SBDI->numBoids = numBoids;

		if (!SBDI->GetBuffer().IsValid()
			&& readRef.IsValid())
		{
			SBDI->SetBuffer(readRef);
		}
	}
}