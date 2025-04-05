// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#pragma once

#include "NiagaraCommon.h"
#include "NiagaraShared.h"
#include "NiagaraDataInterface.h"
#include "RenderGraphFwd.h"
#include "NiagaraDataInterfaceStructuredBufferLegacy.generated.h"

struct FNiagaraDataInterfaceGeneratedFunction;
struct FNiagaraFunctionSignature;

UCLASS(EditInlineNew, Category = "Rendering", meta = (DisplayName = "StructuredBufferLegacy"))
class COMPUTERPLEGACYEXAMPLE_API UNiagaraDataInterfaceStructuredBufferLegacy : public UNiagaraDataInterface
{
	GENERATED_UCLASS_BODY()

	BEGIN_SHADER_PARAMETER_STRUCT(FShaderParameters, )
		SHADER_PARAMETER_SRV(StructuredBuffer<FBoidItem>, boidsIn)
		SHADER_PARAMETER(int32, numBoids)
	END_SHADER_PARAMETER_STRUCT()

public:
	int32 numBoids = 0;

private:
	FShaderResourceViewRHIRef readRef = nullptr;

public:
	 void SetBuffer(FShaderResourceViewRHIRef InBuffer)
	 {
		 readRef = InBuffer;
	 }

	 FShaderResourceViewRHIRef GetBuffer()
	 {
		return readRef;
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

struct FNDIStructuredBufferInstanceDataLegacy_GameThread
{
	FShaderResourceViewRHIRef readRef = nullptr;
	int32 numBoids = 0;

	~FNDIStructuredBufferInstanceDataLegacy_GameThread()
	{
		ReleaseData();
	}

	void ReleaseData()
	{
		numBoids = 0;
		readRef.SafeRelease();
	}
};

//////////////////////////////////////////////////////////////////////////

struct FNDIStructuredBufferInstanceDataLegacy_RenderThread
{
	FShaderResourceViewRHIRef readRef;
	int32 numBoids = 0;

	~FNDIStructuredBufferInstanceDataLegacy_RenderThread()
	{
		ReleaseData();
	}

	void UpdateData(FNDIStructuredBufferInstanceDataLegacy_GameThread& InstanceData_GT)
	{
		ReleaseData();
		readRef = InstanceData_GT.readRef;
		numBoids = InstanceData_GT.numBoids;
	}

	void ReleaseData()
	{
		numBoids = 0;
		readRef.SafeRelease();
	}
};

struct FNDIStructuredBufferProxyLegacy : public FNiagaraDataInterfaceProxy
{
	virtual void ConsumePerInstanceDataFromGameThread(void* PerInstanceData, const FNiagaraSystemInstanceID& Instance) override {}
	virtual int32 PerInstanceDataPassedToRenderThreadSize() const override { return 0; }

	// List of proxy data for each system instance
	TMap<FNiagaraSystemInstanceID, FNDIStructuredBufferInstanceDataLegacy_RenderThread> SystemInstancesToProxyData_RT;
};