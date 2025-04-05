// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#include "Niagara/NDIStructuredBufferFunctionLibrary.h"
#include "RenderGraphResources.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraDataInterfaceStructuredBuffer.h"

void UNDIStructuredBufferFunctionLibrary::SetNiagaraStructuredBuffer(UNiagaraComponent* NiagaraComponent, FName OverrideName, int32 numBoids, TRefCountPtr<FRDGPooledBuffer> InBuffer)
{
	if (UNiagaraDataInterfaceStructuredBuffer* SBDI = UNiagaraFunctionLibrary::GetDataInterface<UNiagaraDataInterfaceStructuredBuffer>(NiagaraComponent, OverrideName))
	{
		SBDI->SetData(numBoids, InBuffer);
	}
}