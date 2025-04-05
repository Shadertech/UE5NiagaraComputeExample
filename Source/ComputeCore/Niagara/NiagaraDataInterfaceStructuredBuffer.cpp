// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#include "NiagaraDataInterfaceStructuredBuffer.h"

#include "NiagaraGpuComputeDispatchInterface.h"
#include "NiagaraCompileHashVisitor.h"
#include "NiagaraTypes.h"
#include "NiagaraShaderParametersBuilder.h"
#include "NiagaraSystemInstance.h"
#include "RenderGraphBuilder.h"
#include "Utils/ComputeFunctionLibrary.h"

static const TCHAR* TemplateShaderFile = TEXT("/ComputeCore/Niagara/NiagaraDataInterfaceStructuredBufferTemplate.ush");

struct FStructuredBufferAttribute
{
	FStructuredBufferAttribute(const TCHAR* InAttributeName, FNiagaraTypeDefinition InTypeDef, FText InDescription)
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

static TConstArrayView<FStructuredBufferAttribute> GetStructuredBufferAttributes()
{
	static const TArray<FStructuredBufferAttribute> StructuredBufferAttributes =
	{
		FStructuredBufferAttribute(TEXT("BoidsPosition"),		FNiagaraTypeDefinition::GetVec3Def(), FText::GetEmpty()),
		FStructuredBufferAttribute(TEXT("BoidsVelocity"),		FNiagaraTypeDefinition::GetVec3Def(), FText::GetEmpty()),
	};

	return MakeArrayView(StructuredBufferAttributes);
}

UNiagaraDataInterfaceStructuredBuffer::UNiagaraDataInterfaceStructuredBuffer(FObjectInitializer const& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Proxy.Reset(new FNDIStructuredBufferProxy());
}

void UNiagaraDataInterfaceStructuredBuffer::PostInitProperties()
{
	Super::PostInitProperties();

	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		ENiagaraTypeRegistryFlags Flags = ENiagaraTypeRegistryFlags::AllowAnyVariable | ENiagaraTypeRegistryFlags::AllowParameter;
		FNiagaraTypeRegistry::Register(FNiagaraTypeDefinition(GetClass()), Flags);
	}
}

void UNiagaraDataInterfaceStructuredBuffer::GetFunctions(TArray<FNiagaraFunctionSignature>& OutFunctions)
{
	TConstArrayView<FStructuredBufferAttribute> StructuredBufferAttributes = GetStructuredBufferAttributes();

	OutFunctions.Reserve(OutFunctions.Num() + (StructuredBufferAttributes.Num()));

	for (const FStructuredBufferAttribute& Attribute : StructuredBufferAttributes)
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
		Signature.Inputs.Add(FNiagaraVariable(FNiagaraTypeDefinition(GetClass()), TEXT("StructuredBufferInterface")));
		Signature.Inputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetIntDef(), TEXT("Id")));
		Signature.Outputs.Add(FNiagaraVariable(FNiagaraTypeDefinition::GetBoolDef(), TEXT("IsValid")));
		Signature.Outputs.Add(FNiagaraVariable(Attribute.TypeDef, Attribute.AttributeName));
	}
}

void UNiagaraDataInterfaceStructuredBuffer::BuildShaderParameters(FNiagaraShaderParametersBuilder& ShaderParametersBuilder) const
{
	ShaderParametersBuilder.AddNestedStruct<FShaderParameters>();
}

void UNiagaraDataInterfaceStructuredBuffer::SetShaderParameters(const FNiagaraDataInterfaceSetShaderParametersContext& Context) const
{
	check(IsInRenderingThread());

	FShaderParameters* Parameters = Context.GetParameterNestedStruct<FShaderParameters>();
	FRDGBuilder& GraphBuilder = Context.GetGraphBuilder();

	FNDIStructuredBufferProxy& DataInterfaceProxy = Context.GetProxy<FNDIStructuredBufferProxy>();
	if (FNDIStructuredBufferInstanceData_RenderThread* InstanceData_RT = DataInterfaceProxy.SystemInstancesToProxyData_RT.Find(Context.GetSystemInstanceID()))
	{
		if (InstanceData_RT->ReadPooled != nullptr
			&& InstanceData_RT->numBoids > 0)
		{
			UComputeFunctionLibrary::RegisterSRV(GraphBuilder, InstanceData_RT->ReadPooled, TEXT("Niagara_BoidsIn_StructuredBuffer"), InstanceData_RT->ReadScopedRef, InstanceData_RT->ReadScopedSRV);
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
bool UNiagaraDataInterfaceStructuredBuffer::AppendCompileHash(FNiagaraCompileHashVisitor* InVisitor) const
{
	if (!Super::AppendCompileHash(InVisitor))
	{
		return false;
	}

	InVisitor->UpdateShaderFile(TemplateShaderFile);
	InVisitor->UpdateShaderParameters<FShaderParameters>();
	return true;
}

void UNiagaraDataInterfaceStructuredBuffer::GetParameterDefinitionHLSL(const FNiagaraDataInterfaceGPUParamInfo& ParamInfo, FString& OutHLSL)
{
	const TMap<FString, FStringFormatArg> TemplateArgs =
	{
		{TEXT("ParameterName"),	ParamInfo.DataInterfaceHLSLSymbol},
	};
	AppendTemplateHLSL(OutHLSL, TemplateShaderFile, TemplateArgs);
}

bool UNiagaraDataInterfaceStructuredBuffer::GetFunctionHLSL(const FNiagaraDataInterfaceGPUParamInfo& ParamInfo, const FNiagaraDataInterfaceGeneratedFunction& FunctionInfo, int FunctionInstanceIndex, FString& OutHLSL)
{
	for (const FStructuredBufferAttribute& Attribute : GetStructuredBufferAttributes())
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
bool UNiagaraDataInterfaceStructuredBuffer::PerInstanceTick(void* PerInstanceData, FNiagaraSystemInstance* SystemInstance, float DeltaSeconds)
{
	FNDIStructuredBufferInstanceData_GameThread* InstanceData = static_cast<FNDIStructuredBufferInstanceData_GameThread*>(PerInstanceData);

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
	FNDIStructuredBufferProxy* RT_Proxy = GetProxyAs<FNDIStructuredBufferProxy>();
	ENQUEUE_RENDER_COMMAND(NDISB_UpdateInstance)(
		[RT_Proxy, RT_InstanceID = SystemInstance->GetId(), RT_InstanceData = InstanceData](FRHICommandListImmediate& RHICmdList)
		{
			FNDIStructuredBufferInstanceData_RenderThread* InstanceData = &RT_Proxy->SystemInstancesToProxyData_RT.FindOrAdd(RT_InstanceID);
			InstanceData->UpdateData(*RT_InstanceData);
		}
	);

	return false;
}

int32 UNiagaraDataInterfaceStructuredBuffer::PerInstanceDataSize() const
{
	return sizeof(FNDIStructuredBufferInstanceData_GameThread);
}

bool UNiagaraDataInterfaceStructuredBuffer::InitPerInstanceData(void* PerInstanceData, FNiagaraSystemInstance* SystemInstance)
{
	check(Proxy);

	FNDIStructuredBufferInstanceData_GameThread* InstanceData = new (PerInstanceData) FNDIStructuredBufferInstanceData_GameThread();
	return true;
}

void UNiagaraDataInterfaceStructuredBuffer::DestroyPerInstanceData(void* PerInstanceData, FNiagaraSystemInstance* SystemInstance)
{
	FNDIStructuredBufferInstanceData_GameThread* InstanceData = static_cast<FNDIStructuredBufferInstanceData_GameThread*>(PerInstanceData);
	InstanceData->~FNDIStructuredBufferInstanceData_GameThread();

	FNDIStructuredBufferProxy* RT_Proxy = GetProxyAs<FNDIStructuredBufferProxy>();
	ENQUEUE_RENDER_COMMAND(FNDISB_DestroyInstance) (
		[RT_Proxy, InstanceID = SystemInstance->GetId()](FRHICommandListImmediate& CmdList)
		{
			RT_Proxy->SystemInstancesToProxyData_RT.Remove(InstanceID);
		}
	);
}