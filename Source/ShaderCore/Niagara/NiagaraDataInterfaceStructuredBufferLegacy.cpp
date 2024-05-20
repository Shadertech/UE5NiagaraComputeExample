#include "NiagaraDataInterfaceStructuredBufferLegacy.h"

#include "NiagaraGpuComputeDispatchInterface.h"
#include "NiagaraCompileHashVisitor.h"
#include "NiagaraTypes.h"
#include "NiagaraShaderParametersBuilder.h"
#include "NiagaraSystemInstance.h"
#include "RenderGraphBuilder.h"

#include "Data/BoidsLib.h"

static const TCHAR* TemplateShaderFileLegacy = TEXT("/ComputeExample/Niagara/NiagaraDataInterfaceStructuredBufferTemplate.ush");

struct FStructuredBufferAttributeLegacy
{
	FStructuredBufferAttributeLegacy(const TCHAR* InAttributeName, const TCHAR* InAttributeType, FNiagaraTypeDefinition InTypeDef, FText InDescription)
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

static TConstArrayView<FStructuredBufferAttributeLegacy> GetStructuredBufferAttributesLegacy()
{
	static const TArray<FStructuredBufferAttributeLegacy> StructuredBufferAttributes =
	{
		FStructuredBufferAttributeLegacy(TEXT("BoidsPosition"),		TEXT("float3"),	FNiagaraTypeDefinition::GetVec3Def(), FText::GetEmpty()),
		FStructuredBufferAttributeLegacy(TEXT("BoidsVelocity"),		TEXT("float3"),	FNiagaraTypeDefinition::GetVec3Def(), FText::GetEmpty()),
	};

	return MakeArrayView(StructuredBufferAttributes);
}

UNiagaraDataInterfaceStructuredBufferLegacy::UNiagaraDataInterfaceStructuredBufferLegacy(FObjectInitializer const& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Proxy.Reset(new FNDIStructuredBufferProxyLegacy());
}

void UNiagaraDataInterfaceStructuredBufferLegacy::PostInitProperties()
{
	Super::PostInitProperties();

	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		ENiagaraTypeRegistryFlags Flags = ENiagaraTypeRegistryFlags::AllowAnyVariable | ENiagaraTypeRegistryFlags::AllowParameter;
		FNiagaraTypeRegistry::Register(FNiagaraTypeDefinition(GetClass()), Flags);
	}
}

void UNiagaraDataInterfaceStructuredBufferLegacy::GetFunctions(TArray<FNiagaraFunctionSignature>& OutFunctions)
{
	TConstArrayView<FStructuredBufferAttributeLegacy> StructuredBufferAttributes = GetStructuredBufferAttributesLegacy();

	OutFunctions.Reserve(OutFunctions.Num() + (StructuredBufferAttributes.Num()));

	for (const FStructuredBufferAttributeLegacy& Attribute : StructuredBufferAttributes)
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

void UNiagaraDataInterfaceStructuredBufferLegacy::BuildShaderParameters(FNiagaraShaderParametersBuilder& ShaderParametersBuilder) const
{
	ShaderParametersBuilder.AddNestedStruct<FShaderParameters>();
}

void UNiagaraDataInterfaceStructuredBufferLegacy::SetShaderParameters(const FNiagaraDataInterfaceSetShaderParametersContext& Context) const
{
	check(IsInRenderingThread());

	FShaderParameters* Parameters = Context.GetParameterNestedStruct<FShaderParameters>();
	FRDGBuilder& GraphBuilder = Context.GetGraphBuilder();

	FNDIStructuredBufferProxyLegacy& DataInterfaceProxy = Context.GetProxy<FNDIStructuredBufferProxyLegacy>();
	if (FNDIStructuredBufferInstanceDataLegacy_RenderThread* InstanceData_RT = DataInterfaceProxy.SystemInstancesToProxyData_RT.Find(Context.GetSystemInstanceID()))
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
bool UNiagaraDataInterfaceStructuredBufferLegacy::AppendCompileHash(FNiagaraCompileHashVisitor* InVisitor) const
{
	if (!Super::AppendCompileHash(InVisitor))
	{
		return false;
	}

	InVisitor->UpdateShaderFile(TemplateShaderFileLegacy);
	InVisitor->UpdateShaderParameters<FShaderParameters>();
	return true;
}

void UNiagaraDataInterfaceStructuredBufferLegacy::GetParameterDefinitionHLSL(const FNiagaraDataInterfaceGPUParamInfo& ParamInfo, FString& OutHLSL)
{
	const TMap<FString, FStringFormatArg> TemplateArgs =
	{
		{TEXT("ParameterName"),	ParamInfo.DataInterfaceHLSLSymbol},
	};
	AppendTemplateHLSL(OutHLSL, TemplateShaderFileLegacy, TemplateArgs);
}

bool UNiagaraDataInterfaceStructuredBufferLegacy::GetFunctionHLSL(const FNiagaraDataInterfaceGPUParamInfo& ParamInfo, const FNiagaraDataInterfaceGeneratedFunction& FunctionInfo, int FunctionInstanceIndex, FString& OutHLSL)
{
	for (const FStructuredBufferAttributeLegacy& Attribute : GetStructuredBufferAttributesLegacy())
	{
		if (FunctionInfo.DefinitionName == Attribute.DisplayFunctionName)
		{
			return true;
		}
	}

	return false;
}

// This ticks on the game thread and lets us do work to initialize the instance data.
// If you need to do work on the gathered instance data after the simulation is done, use PerInstanceTickPostSimulate() instead. 
bool UNiagaraDataInterfaceStructuredBufferLegacy::PerInstanceTick(void* PerInstanceData, FNiagaraSystemInstance* SystemInstance, float DeltaSeconds)
{
	FNDIStructuredBufferInstanceDataLegacy_GameThread* InstanceData = static_cast<FNDIStructuredBufferInstanceDataLegacy_GameThread*>(PerInstanceData);

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

	FNDIStructuredBufferProxyLegacy* RT_Proxy = GetProxyAs<FNDIStructuredBufferProxyLegacy>();
	ENQUEUE_RENDER_COMMAND(NDISB_UpdateInstance)(
		[RT_Proxy, RT_InstanceID = SystemInstance->GetId(), RT_InstanceData = SystemInstance->GetId(), RT_NumBoids = numBoids, RT_ReadRef = readRef](FRHICommandListImmediate& RHICmdList)
		{
			FNDIStructuredBufferInstanceDataLegacy_RenderThread* InstanceData = &RT_Proxy->SystemInstancesToProxyData_RT.FindOrAdd(RT_InstanceID);
			InstanceData->numBoids = RT_NumBoids;
			InstanceData->readRef = RT_ReadRef;
		}
	);

	return false;
}

int32 UNiagaraDataInterfaceStructuredBufferLegacy::PerInstanceDataSize() const
{
	return sizeof(FNDIStructuredBufferInstanceDataLegacy_GameThread);
}

bool UNiagaraDataInterfaceStructuredBufferLegacy::InitPerInstanceData(void* PerInstanceData, FNiagaraSystemInstance* SystemInstance)
{
	check(Proxy);

	FNDIStructuredBufferInstanceDataLegacy_GameThread* InstanceData = new (PerInstanceData) FNDIStructuredBufferInstanceDataLegacy_GameThread();
	InstanceData->numBoids = numBoids;
	InstanceData->readRef = readRef;
	return true;
}


void UNiagaraDataInterfaceStructuredBufferLegacy::DestroyPerInstanceData(void* PerInstanceData, FNiagaraSystemInstance* SystemInstance)
{
	FNDIStructuredBufferInstanceDataLegacy_GameThread* InstanceData = static_cast<FNDIStructuredBufferInstanceDataLegacy_GameThread*>(PerInstanceData);
	InstanceData->~FNDIStructuredBufferInstanceDataLegacy_GameThread();

	FNDIStructuredBufferProxyLegacy* RT_Proxy = GetProxyAs<FNDIStructuredBufferProxyLegacy>();
	ENQUEUE_RENDER_COMMAND(FNDISB_DestroyInstance) (
		[RT_Proxy, InstanceID = SystemInstance->GetId()](FRHICommandListImmediate& CmdList)
		{
			RT_Proxy->SystemInstancesToProxyData_RT.Remove(InstanceID);
		}
	);
}
#endif