// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#include "Niagara/ST_NDIBoidsExampleBufferFunctionLibrary.h"
#include "RenderGraphResources.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Niagara/ST_NiagaraDataInterfaceBoidsExampleBuffer.h"

void UST_NDIBoidsExampleBufferFunctionLibrary::SetNiagaraBoidsExampleBuffer(UNiagaraComponent* NiagaraComponent, FName OverrideName, int32 numBoids, TRefCountPtr<FRDGPooledBuffer> InBuffer)
{
	if (UST_NiagaraDataInterfaceBoidsExampleBuffer* SBDI = UNiagaraFunctionLibrary::GetDataInterface<UST_NiagaraDataInterfaceBoidsExampleBuffer>(NiagaraComponent, OverrideName))
	{
		SBDI->SetData(numBoids, InBuffer);
	}
}