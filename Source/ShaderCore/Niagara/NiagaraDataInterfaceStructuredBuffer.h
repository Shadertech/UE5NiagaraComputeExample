// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "NiagaraCommon.h"
#include "NiagaraShared.h"
#include "NiagaraDataInterface.h"
#include "RenderGraphFwd.h"
#include "NiagaraDataInterfaceStructuredBuffer.generated.h"

struct FNiagaraDataInterfaceGeneratedFunction;
struct FNiagaraFunctionSignature;

UCLASS(EditInlineNew, Category = "Rendering", meta = (DisplayName = "StructuredBuffer"))
class SHADERCORE_API UNiagaraDataInterfaceStructuredBuffer : public UNiagaraDataInterface
{
	GENERATED_UCLASS_BODY()

	BEGIN_SHADER_PARAMETER_STRUCT(FShaderParameters, )
		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<FBoidItem>, boidsIn)
		SHADER_PARAMETER(int32, numBoids)
	END_SHADER_PARAMETER_STRUCT()

public:
	int32 numBoids;

private:
	TRefCountPtr<FRDGPooledBuffer> ReadPooled;

public:
	 void SetBuffer(TRefCountPtr<FRDGPooledBuffer> InBuffer)
	 {
		 ReadPooled = InBuffer;
	 }

	 TRefCountPtr<FRDGPooledBuffer> GetBuffer()
	 {
		return ReadPooled;
	 }

	//UObject Interface
	virtual void PostInitProperties() override;
	//UObject Interface End

	//UNiagaraDataInterface Interface
	virtual void GetFunctions(TArray<FNiagaraFunctionSignature>& OutFunctions)override;
	virtual bool CanExecuteOnTarget(ENiagaraSimTarget Target) const override { return Target == ENiagaraSimTarget::GPUComputeSim; }

	virtual void BuildShaderParameters(FNiagaraShaderParametersBuilder& ShaderParametersBuilder) const override;
	virtual void SetShaderParameters(const FNiagaraDataInterfaceSetShaderParametersContext& Context) const override;

#if WITH_EDITORONLY_DATA
	virtual bool AppendCompileHash(FNiagaraCompileHashVisitor* InVisitor) const override;
	virtual void GetParameterDefinitionHLSL(const FNiagaraDataInterfaceGPUParamInfo& ParamInfo, FString& OutHLSL) override;
	virtual bool GetFunctionHLSL(const FNiagaraDataInterfaceGPUParamInfo& ParamInfo, const FNiagaraDataInterfaceGeneratedFunction& FunctionInfo, int FunctionInstanceIndex, FString& OutHLSL) override;
#endif

	virtual bool HasPreSimulateTick() const override { return true; }
	virtual bool PerInstanceTick(void* PerInstanceData, FNiagaraSystemInstance* SystemInstance, float DeltaSeconds) override;
	virtual int32 PerInstanceDataSize() const override;
	virtual bool InitPerInstanceData(void* PerInstanceData, FNiagaraSystemInstance* SystemInstance) override;
	virtual void DestroyPerInstanceData(void* PerInstanceData, FNiagaraSystemInstance* SystemInstance) override;
	//UNiagaraDataInterface Interface
};

struct FNDIStructuredBufferInstanceData_GameThread
{
	//FNiagaraParameterDirectBinding<UObject*> UserParamBinding;
	TRefCountPtr<FRDGPooledBuffer> ReadPooled;
	//FRDGBufferSRVRef ReadScopedSRV;
	int32 numBoids;
};

//////////////////////////////////////////////////////////////////////////

struct FNDIStructuredBufferInstanceData_RenderThread
{
	TRefCountPtr<FRDGPooledBuffer> ReadPooled;
	//FRDGBufferSRVRef ReadScopedSRV;
	int32 numBoids;
};

struct FNDIStructuredBufferProxy : public FNiagaraDataInterfaceProxy
{
	virtual void ConsumePerInstanceDataFromGameThread(void* PerInstanceData, const FNiagaraSystemInstanceID& Instance) override {}
	virtual int32 PerInstanceDataPassedToRenderThreadSize() const override { return 0; }

	// List of proxy data for each system instance
	TMap<FNiagaraSystemInstanceID, FNDIStructuredBufferInstanceData_RenderThread> SystemInstancesToProxyData_RT;
};