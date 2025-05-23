// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#include "Niagara/ST_NDIBoidsExampleBufferLegacy.h"

#include "GlobalRenderResources.h"
#include "NiagaraGpuComputeDispatchInterface.h"
#include "NiagaraCompileHashVisitor.h"
#include "NiagaraTypes.h"
#include "NiagaraShaderParametersBuilder.h"
#include "NiagaraSystemInstance.h"
#include "RenderGraphBuilder.h"

#include "Data/ST_BoidsLib.h"

static const TCHAR* TemplateShaderFileLegacy = TEXT("/ComputeCore/Shared/Niagara/NDIBoidsExampleBufferTemplate.ush");

struct FST_BoidsExampleBufferAttributeLegacy
{
	FST_BoidsExampleBufferAttributeLegacy(const TCHAR* InAttributeName, const TCHAR* InAttributeType, FNiagaraTypeDefinition InTypeDef, FText InDescription)
		: AttributeName(InAttributeName)
		, AttributeType(InAttributeType)
		, TypeDef(InTypeDef)
		, Description(InDescription)
	{
		FString TempName;
		TempName = TEXT("Sample");
		TempName += AttributeName;
		DisplayFunctionName = FName(TempName);
	}

	const TCHAR* AttributeName;
	const TCHAR* AttributeType;
	FName					DisplayFunctionName;
	FNiagaraTypeDefinition	TypeDef;
	FText					Description;
};

static TConstArrayView<FST_BoidsExampleBufferAttributeLegacy> GetBoidsExampleBufferAttributesLegacy()
{
	static const TArray<FST_BoidsExampleBufferAttributeLegacy> BoidsExampleBufferAttributes =
	{
		FST_BoidsExampleBufferAttributeLegacy(TEXT("BoidsPosition"),		TEXT("float3"),	FNiagaraTypeDefinition::GetVec3Def(), FText::GetEmpty()),
		FST_BoidsExampleBufferAttributeLegacy(TEXT("BoidsVelocity"),		TEXT("float3"),	FNiagaraTypeDefinition::GetVec3Def(), FText::GetEmpty()),
	};

	return MakeArrayView(BoidsExampleBufferAttributes);
}

UST_NDIBoidsExampleBufferLegacy::UST_NDIBoidsExampleBufferLegacy(FObjectInitializer const& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Proxy.Reset(new FST_NDIBoidsExampleBufferProxyLegacy());
}

void UST_NDIBoidsExampleBufferLegacy::PostInitProperties()
{
	Super::PostInitProperties();

	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		ENiagaraTypeRegistryFlags Flags = ENiagaraTypeRegistryFlags::AllowAnyVariable | ENiagaraTypeRegistryFlags::AllowParameter;
		FNiagaraTypeRegistry::Register(FNiagaraTypeDefinition(GetClass()), Flags);
	}
}

void UST_NDIBoidsExampleBufferLegacy::GetFunctions(TArray<FNiagaraFunctionSignature>& OutFunctions)
{
	TConstArrayView<FST_BoidsExampleBufferAttributeLegacy> BoidsExampleBufferAttributes = GetBoidsExampleBufferAttributesLegacy();

	OutFunctions.Reserve(OutFunctions.Num() + (BoidsExampleBufferAttributes.Num()));

	for (const FST_BoidsExampleBufferAttributeLegacy& Attribute : BoidsExampleBufferAttributes)
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

void UST_NDIBoidsExampleBufferLegacy::BuildShaderParameters(FNiagaraShaderParametersBuilder& ShaderParametersBuilder) const
{
	ShaderParametersBuilder.AddNestedStruct<FShaderParameters>();
}

void UST_NDIBoidsExampleBufferLegacy::SetShaderParameters(const FNiagaraDataInterfaceSetShaderParametersContext& Context) const
{
	check(IsInRenderingThread());

	FShaderParameters* Parameters = Context.GetParameterNestedStruct<FShaderParameters>();
	FRDGBuilder& GraphBuilder = Context.GetGraphBuilder();

	FST_NDIBoidsExampleBufferProxyLegacy& DataInterfaceProxy = Context.GetProxy<FST_NDIBoidsExampleBufferProxyLegacy>();
	if (FST_NDIBoidsExampleBufferInstanceDataLegacy_RenderThread* InstanceData_RT = DataInterfaceProxy.SystemInstancesToProxyData_RT.Find(Context.GetSystemInstanceID()))
	{
		if (InstanceData_RT->readRef != nullptr
			&& InstanceData_RT->numBoids > 0)
		{
			Parameters->boidsIn = InstanceData_RT->readRef;
			Parameters->numBoids = InstanceData_RT->numBoids;
			return;
		}
	}
	
	Parameters->boidsIn = GWhiteVertexBufferWithSRV->ShaderResourceViewRHI.GetReference();
	Parameters->numBoids = 0;
}

#if WITH_EDITORONLY_DATA
// this lets the niagara compiler know that it needs to recompile an effect when our hlsl file changes
bool UST_NDIBoidsExampleBufferLegacy::AppendCompileHash(FNiagaraCompileHashVisitor* InVisitor) const
{
	if (!Super::AppendCompileHash(InVisitor))
	{
		return false;
	}

	InVisitor->UpdateShaderFile(TemplateShaderFileLegacy);
	InVisitor->UpdateShaderParameters<FShaderParameters>();
	return true;
}

void UST_NDIBoidsExampleBufferLegacy::GetParameterDefinitionHLSL(const FNiagaraDataInterfaceGPUParamInfo& ParamInfo, FString& OutHLSL)
{
	const TMap<FString, FStringFormatArg> TemplateArgs =
	{
		{TEXT("ParameterName"),	ParamInfo.DataInterfaceHLSLSymbol},
	};
	AppendTemplateHLSL(OutHLSL, TemplateShaderFileLegacy, TemplateArgs);
}

bool UST_NDIBoidsExampleBufferLegacy::GetFunctionHLSL(const FNiagaraDataInterfaceGPUParamInfo& ParamInfo, const FNiagaraDataInterfaceGeneratedFunction& FunctionInfo, int FunctionInstanceIndex, FString& OutHLSL)
{
	for (const FST_BoidsExampleBufferAttributeLegacy& Attribute : GetBoidsExampleBufferAttributesLegacy())
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
bool UST_NDIBoidsExampleBufferLegacy::PerInstanceTick(void* PerInstanceData, FNiagaraSystemInstance* SystemInstance, float DeltaSeconds)
{
	FST_NDIBoidsExampleBufferInstanceDataLegacy_GameThread* InstanceData = static_cast<FST_NDIBoidsExampleBufferInstanceDataLegacy_GameThread*>(PerInstanceData);

	if (!InstanceData)
	{
		return true;
	}

	if (InstanceData->numBoids == numBoids && InstanceData->readRef == readRef)
	{
		return false;
	}

	InstanceData->numBoids = numBoids;
	InstanceData->readRef = readRef;

	FST_NDIBoidsExampleBufferProxyLegacy* RT_Proxy = GetProxyAs<FST_NDIBoidsExampleBufferProxyLegacy>();
	ENQUEUE_RENDER_COMMAND(NDISB_UpdateInstance)(
		[RT_Proxy, RT_InstanceID = SystemInstance->GetId(), RT_InstanceData = InstanceData](FRHICommandListImmediate& RHICmdList)
		{
			FST_NDIBoidsExampleBufferInstanceDataLegacy_RenderThread* InstanceData = &RT_Proxy->SystemInstancesToProxyData_RT.FindOrAdd(RT_InstanceID);
			InstanceData->UpdateData(*RT_InstanceData);
		}
	);

	return false;
}

int32 UST_NDIBoidsExampleBufferLegacy::PerInstanceDataSize() const
{
	return sizeof(FST_NDIBoidsExampleBufferInstanceDataLegacy_GameThread);
}

bool UST_NDIBoidsExampleBufferLegacy::InitPerInstanceData(void* PerInstanceData, FNiagaraSystemInstance* SystemInstance)
{
	check(Proxy);

	FST_NDIBoidsExampleBufferInstanceDataLegacy_GameThread* InstanceData = new (PerInstanceData) FST_NDIBoidsExampleBufferInstanceDataLegacy_GameThread();
	InstanceData->numBoids = numBoids;
	InstanceData->readRef = readRef;
	return true;
}


void UST_NDIBoidsExampleBufferLegacy::DestroyPerInstanceData(void* PerInstanceData, FNiagaraSystemInstance* SystemInstance)
{
	FST_NDIBoidsExampleBufferInstanceDataLegacy_GameThread* InstanceData = static_cast<FST_NDIBoidsExampleBufferInstanceDataLegacy_GameThread*>(PerInstanceData);
	InstanceData->~FST_NDIBoidsExampleBufferInstanceDataLegacy_GameThread();

	FST_NDIBoidsExampleBufferProxyLegacy* RT_Proxy = GetProxyAs<FST_NDIBoidsExampleBufferProxyLegacy>();
	ENQUEUE_RENDER_COMMAND(FNDISB_DestroyInstance) (
		[RT_Proxy, InstanceID = SystemInstance->GetId()](FRHICommandListImmediate& CmdList)
		{
			RT_Proxy->SystemInstancesToProxyData_RT.Remove(InstanceID);
		}
	);
}