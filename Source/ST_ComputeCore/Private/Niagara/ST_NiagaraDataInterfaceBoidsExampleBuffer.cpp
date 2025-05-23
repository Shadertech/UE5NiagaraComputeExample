// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#include "Niagara/ST_NiagaraDataInterfaceBoidsExampleBuffer.h"

#include "NiagaraGpuComputeDispatchInterface.h"
#include "NiagaraCompileHashVisitor.h"
#include "NiagaraTypes.h"
#include "NiagaraShaderParametersBuilder.h"
#include "NiagaraSystemInstance.h"
#include "RenderGraphBuilder.h"
#include "Utils/ST_ComputeFunctionLibrary.h"

static const TCHAR* TemplateShaderFile = TEXT("/ComputeCore/Shared/Niagara/NDIBoidsExampleBufferTemplate.ush");

struct FST_BoidsExampleBufferAttribute
{
	FST_BoidsExampleBufferAttribute(const TCHAR* InAttributeName, FNiagaraTypeDefinition InTypeDef, FText InDescription)
		: AttributeName(InAttributeName)
		, TypeDef(InTypeDef)
		, Description(InDescription)
	{
		FString TempName;
		TempName = TEXT("Sample");
		TempName += AttributeName;
		DisplayFunctionName = FName(TempName);
	}

	const TCHAR* AttributeName;
	FName					DisplayFunctionName;
	FNiagaraTypeDefinition	TypeDef;
	FText					Description;
};

static TConstArrayView<FST_BoidsExampleBufferAttribute> GetBoidsExampleBufferAttributes()
{
	static const TArray<FST_BoidsExampleBufferAttribute> BoidsExampleBufferAttributes =
	{
		FST_BoidsExampleBufferAttribute(TEXT("BoidsPosition"),		FNiagaraTypeDefinition::GetVec3Def(), FText::GetEmpty()),
		FST_BoidsExampleBufferAttribute(TEXT("BoidsVelocity"),		FNiagaraTypeDefinition::GetVec3Def(), FText::GetEmpty()),
	};

	return MakeArrayView(BoidsExampleBufferAttributes);
}

UST_NiagaraDataInterfaceBoidsExampleBuffer::UST_NiagaraDataInterfaceBoidsExampleBuffer(FObjectInitializer const& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Proxy.Reset(new FST_NDIBoidsExampleBufferProxy());
}

void UST_NiagaraDataInterfaceBoidsExampleBuffer::PostInitProperties()
{
	Super::PostInitProperties();

	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		ENiagaraTypeRegistryFlags Flags = ENiagaraTypeRegistryFlags::AllowAnyVariable | ENiagaraTypeRegistryFlags::AllowParameter;
		FNiagaraTypeRegistry::Register(FNiagaraTypeDefinition(GetClass()), Flags);
	}
}

void UST_NiagaraDataInterfaceBoidsExampleBuffer::GetFunctions(TArray<FNiagaraFunctionSignature>& OutFunctions)
{
	TConstArrayView<FST_BoidsExampleBufferAttribute> BoidsExampleBufferAttributes = GetBoidsExampleBufferAttributes();

	OutFunctions.Reserve(OutFunctions.Num() + (BoidsExampleBufferAttributes.Num()));

	for (const FST_BoidsExampleBufferAttribute& Attribute : BoidsExampleBufferAttributes)
	{
		FNiagaraFunctionSignature& Signature = OutFunctions.AddDefaulted_GetRef();
		Signature.Name = Attribute.DisplayFunctionName;
#if WITH_EDITORONLY_DATA
		Signature.Description = Attribute.Description;
#endif
		Signature.bMemberFunction = true;
		Signature.bRequiresContext = false;
		Signature.bSupportsCPU = false;
		Signature.bExperimental = true;
		Signature.Inputs.Add(FNiagaraVariable(FNiagaraTypeDefinition(GetClass()), TEXT("BoidsExampleBufferInterface")));
		Signature.Inputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetIntDef(), TEXT("Id")));
		Signature.Outputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetBoolDef(), TEXT("IsValid")));
		Signature.Outputs.Add(FNiagaraVariable(Attribute.TypeDef, Attribute.AttributeName));
	}
}

void UST_NiagaraDataInterfaceBoidsExampleBuffer::BuildShaderParameters(FNiagaraShaderParametersBuilder& ShaderParametersBuilder) const
{
	ShaderParametersBuilder.AddNestedStruct<FShaderParameters>();
}

void UST_NiagaraDataInterfaceBoidsExampleBuffer::SetShaderParameters(const FNiagaraDataInterfaceSetShaderParametersContext& Context) const
{
	check(IsInRenderingThread());

	FShaderParameters* Parameters = Context.GetParameterNestedStruct<FShaderParameters>();
	FRDGBuilder& GraphBuilder = Context.GetGraphBuilder();

	FST_NDIBoidsExampleBufferProxy& DataInterfaceProxy = Context.GetProxy<FST_NDIBoidsExampleBufferProxy>();
	if (FST_NDIBoidsExampleBufferInstanceData_RenderThread* InstanceData_RT = DataInterfaceProxy.SystemInstancesToProxyData_RT.Find(Context.GetSystemInstanceID()))
	{
		if (InstanceData_RT->ReadPooled != nullptr
			&& InstanceData_RT->numBoids > 0)
		{
			UST_ComputeFunctionLibrary::RegisterSRV(GraphBuilder, InstanceData_RT->ReadPooled, TEXT("Niagara_BoidsIn_BoidsExampleBuffer"), InstanceData_RT->ReadScopedRef, InstanceData_RT->ReadScopedSRV);
		}

		if (InstanceData_RT->ReadScopedRef != nullptr)
		{
			Parameters->boidsIn = InstanceData_RT->ReadScopedSRV;
			Parameters->numBoids = InstanceData_RT->numBoids;
			return;
		}
	}

	Parameters->boidsIn = Context.GetComputeDispatchInterface().GetEmptyBufferSRV(GraphBuilder, EPixelFormat::PF_A16B16G16R16);
	Parameters->numBoids = 0;
}

#if WITH_EDITORONLY_DATA
// this lets the niagara compiler know that it needs to recompile an effect when our hlsl file changes
bool UST_NiagaraDataInterfaceBoidsExampleBuffer::AppendCompileHash(FNiagaraCompileHashVisitor* InVisitor) const
{
	if (!Super::AppendCompileHash(InVisitor))
	{
		return false;
	}

	InVisitor->UpdateShaderFile(TemplateShaderFile);
	InVisitor->UpdateShaderParameters<FShaderParameters>();
	return true;
}

void UST_NiagaraDataInterfaceBoidsExampleBuffer::GetParameterDefinitionHLSL(const FNiagaraDataInterfaceGPUParamInfo& ParamInfo, FString& OutHLSL)
{
	const TMap<FString, FStringFormatArg> TemplateArgs =
	{
		{TEXT("ParameterName"),	ParamInfo.DataInterfaceHLSLSymbol},
	};
	AppendTemplateHLSL(OutHLSL, TemplateShaderFile, TemplateArgs);
}

bool UST_NiagaraDataInterfaceBoidsExampleBuffer::GetFunctionHLSL(const FNiagaraDataInterfaceGPUParamInfo& ParamInfo, const FNiagaraDataInterfaceGeneratedFunction& FunctionInfo, int FunctionInstanceIndex, FString& OutHLSL)
{
	for (const FST_BoidsExampleBufferAttribute& Attribute : GetBoidsExampleBufferAttributes())
	{
		if (FunctionInfo.DefinitionName == Attribute.DisplayFunctionName)
		{
			return true;
		}
	}

	return false;
}
#endif

// This ticks on the game thread and lets us do work to initialize the instance data.
// If you need to do work on the gathered instance data after the simulation is done, use PerInstanceTickPostSimulate() instead. 
bool UST_NiagaraDataInterfaceBoidsExampleBuffer::PerInstanceTick(void* PerInstanceData, FNiagaraSystemInstance* SystemInstance, float DeltaSeconds)
{
	FST_NDIBoidsExampleBufferInstanceData_GameThread* InstanceData = static_cast<FST_NDIBoidsExampleBufferInstanceData_GameThread*>(PerInstanceData);

	if (!InstanceData)
	{
		return true;
	}

	if (numBoids == 0 || !ReadPooled.IsValid())
	{
		return false;
	}
	if ((InstanceData->numBoids > 0 && InstanceData->numBoids == numBoids)
		&& (InstanceData->ReadPooled.IsValid() && InstanceData->ReadPooled->GetSize() == ReadPooled->GetSize()))
	{
		return false;
	}

	InstanceData->numBoids = numBoids;
	InstanceData->ReadPooled = ReadPooled;
	FST_NDIBoidsExampleBufferProxy* RT_Proxy = GetProxyAs<FST_NDIBoidsExampleBufferProxy>();
	ENQUEUE_RENDER_COMMAND(NDISB_UpdateInstance)(
		[RT_Proxy, RT_InstanceID = SystemInstance->GetId(), RT_InstanceData = InstanceData](FRHICommandListImmediate& RHICmdList)
		{
			FST_NDIBoidsExampleBufferInstanceData_RenderThread* InstanceData = &RT_Proxy->SystemInstancesToProxyData_RT.FindOrAdd(RT_InstanceID);
			InstanceData->UpdateData(*RT_InstanceData);
		}
	);

	return false;
}

int32 UST_NiagaraDataInterfaceBoidsExampleBuffer::PerInstanceDataSize() const
{
	return sizeof(FST_NDIBoidsExampleBufferInstanceData_GameThread);
}

bool UST_NiagaraDataInterfaceBoidsExampleBuffer::InitPerInstanceData(void* PerInstanceData, FNiagaraSystemInstance* SystemInstance)
{
	check(Proxy);

	FST_NDIBoidsExampleBufferInstanceData_GameThread* InstanceData = new (PerInstanceData) FST_NDIBoidsExampleBufferInstanceData_GameThread();
	return true;
}

void UST_NiagaraDataInterfaceBoidsExampleBuffer::DestroyPerInstanceData(void* PerInstanceData, FNiagaraSystemInstance* SystemInstance)
{
	FST_NDIBoidsExampleBufferInstanceData_GameThread* InstanceData = static_cast<FST_NDIBoidsExampleBufferInstanceData_GameThread*>(PerInstanceData);
	InstanceData->~FST_NDIBoidsExampleBufferInstanceData_GameThread();

	FST_NDIBoidsExampleBufferProxy* RT_Proxy = GetProxyAs<FST_NDIBoidsExampleBufferProxy>();
	ENQUEUE_RENDER_COMMAND(FNDISB_DestroyInstance) (
		[RT_Proxy, InstanceID = SystemInstance->GetId()](FRHICommandListImmediate& CmdList)
		{
			RT_Proxy->SystemInstancesToProxyData_RT.Remove(InstanceID);
		}
	);
}