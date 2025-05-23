// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#include "Niagara/ST_NDIBoidsExampleBufferLegacyFunctionLibrary.h"

#include "RHIResources.h"
#include "Engine/World.h"
#include "NiagaraComponent.h"
#include "NiagaraSystemInstance.h"
#include "NiagaraSystemInstanceController.h"
#include "NiagaraFunctionLibrary.h"
#include "Niagara/ST_NDIBoidsExampleBufferLegacy.h"

void UST_NDIBoidsExampleBufferLegacyFunctionLibrary::SetNiagaraBoidsExampleBufferLegacy(UNiagaraComponent* NiagaraComponent, FName OverrideName, int32 numBoids, FShaderResourceViewRHIRef readRef)
{
	if (UST_NDIBoidsExampleBufferLegacy* SBDI = UNiagaraFunctionLibrary::GetDataInterface<UST_NDIBoidsExampleBufferLegacy>(NiagaraComponent, OverrideName))
	{
		SBDI->numBoids = numBoids;

		if (!SBDI->GetBuffer().IsValid()
			&& readRef.IsValid())
		{
			SBDI->SetBuffer(readRef);
		}
	}
}