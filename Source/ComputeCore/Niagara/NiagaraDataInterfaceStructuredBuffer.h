// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#pragma once

#include "NiagaraCommon.h"
#include "NiagaraShared.h"
#include "NiagaraDataInterface.h"
#include "RenderGraphFwd.h"
#include "RenderGraphResources.h"
#include "NiagaraDataInterfaceStructuredBuffer.generated.h"

struct FNiagaraDataInterfaceGeneratedFunction;
struct FNiagaraFunctionSignature;

UCLASS(EditInlineNew, Category = "Rendering", meta = (DisplayName = "StructuredBuffer"))
class COMPUTECORE_API UNiagaraDataInterfaceStructuredBuffer : public UNiagaraDataInterface
{
	GENERATED_UCLASS_BODY()

	BEGIN_SHADER_PARAMETER_STRUCT(FShaderParameters, )
		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<FBoidItem>, boidsIn)
		SHADER_PARAMETER(int32, numBoids)
	END_SHADER_PARAMETER_STRUCT()

private:
	int32 numBoids = 0;
	TRefCountPtr<FRDGPooledBuffer> ReadPooled = nullptr;

public:
	 void SetData(int32 _numBoids, TRefCountPtr<FRDGPooledBuffer> _ReadPooled)
	 {
		 numBoids = _numBoids;
		 ReadPooled = _ReadPooled;
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
	TRefCountPtr<FRDGPooledBuffer> ReadPooled = nullptr;
	int32 numBoids = 0;

	~FNDIStructuredBufferInstanceData_GameThread()
	{
		ReleaseData();
	}

	void ReleaseData()
	{
		numBoids = 0;
		ReadPooled.SafeRelease();
	}
};

//////////////////////////////////////////////////////////////////////////

struct FNDIStructuredBufferInstanceData_RenderThread
{
	TRefCountPtr<FRDGPooledBuffer> ReadPooled = nullptr;
	FRDGBufferRef ReadScopedRef = nullptr;
	FRDGBufferSRVRef ReadScopedSRV = nullptr;
	int32 numBoids = 0;

	~FNDIStructuredBufferInstanceData_RenderThread()
	{
		ReleaseData();
	}

	void UpdateData(FNDIStructuredBufferInstanceData_GameThread& InstanceData_GT)
	{
		ReleaseData();
		ReadPooled = InstanceData_GT.ReadPooled;
		numBoids = InstanceData_GT.numBoids;
	}

	void ReleaseData()
	{
		numBoids = 0;
		ReadPooled.SafeRelease();
	}
};

struct FNDIStructuredBufferProxy : public FNiagaraDataInterfaceProxy
{
	virtual void ConsumePerInstanceDataFromGameThread(void* PerInstanceData, const FNiagaraSystemInstanceID& Instance) override {}
	virtual int32 PerInstanceDataPassedToRenderThreadSize() const override { return 0; }

	// List of proxy data for each system instance
	TMap<FNiagaraSystemInstanceID, FNDIStructuredBufferInstanceData_RenderThread> SystemInstancesToProxyData_RT;
};