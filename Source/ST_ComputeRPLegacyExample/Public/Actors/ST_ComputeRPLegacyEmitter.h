// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/ST_BoidsLib.h"
#include "Utils/ST_PingPongBuffer.h"
#include "NiagaraComponent.h"

#include "Base/ST_ComputeActorBase.h"
#include "RHICommandList.h"
#include "RHIResources.h"
#include "RHIFwd.h"

#include "ST_ComputeRPLegacyEmitter.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogComputeRPLegacyEmitter, Log, All);

UCLASS()
class ST_COMPUTERPLEGACYEXAMPLE_API AST_ComputeRPLegacyEmitter : public AST_ComputeActorBase
{
	GENERATED_BODY()

public:
	AST_ComputeRPLegacyEmitter();

protected:
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

	// Start Managed Compute Shader Interface 
	virtual void InitComputeShader_GameThread() override;
	virtual void InitComputeShader_RenderThread(FRHICommandListImmediate& RHICmdList) override;

	virtual void ExecuteComputeShader_GameThread(float DeltaTime) override;
	virtual void ExecuteComputeShader_RenderThread(FRHICommandListImmediate& RHICmdList) override;

	virtual void DisposeComputeShader_GameThread() override;
	virtual void DisposeComputeShader_RenderThread(FRHICommandListImmediate& RHICmdList) override;
	// End Managed Compute Shader Interface

protected:
	bool SetConstantParameters();
	bool SetDynamicParameters();

	virtual FString GetOwnerName() const override;

protected:
	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "ShaderTech|BoidParameters")
	FST_BoidCurrentParameters BoidCurrentParameters;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ShaderTech|Boids")
	TArray<FST_BoidItem> BoidsArray;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Niagara")
	UNiagaraComponent* Niagara = nullptr;

private:
	FBufferRHIRef readBuffer = nullptr;
	FShaderResourceViewRHIRef readRef = nullptr;
	FBufferRHIRef writeBuffer = nullptr;
	FUnorderedAccessViewRHIRef writeRef = nullptr;

	int32 BoidItemSize = 0;

	FMatrix BoundsMatrix;
};

inline FString AST_ComputeRPLegacyEmitter::GetOwnerName() const
{
	return "ComputeRPLegacyEmitter";
}