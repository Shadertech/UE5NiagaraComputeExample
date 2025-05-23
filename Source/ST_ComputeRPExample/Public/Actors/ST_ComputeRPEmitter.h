// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/ST_BoidsLib.h"
#include "Utils/ST_PingPongBuffer.h"
#include "GlobalShader.h"
#include "NiagaraComponent.h"
#include "DataDrivenShaderPlatformInfo.h"

#include "Base/ST_ComputeActorBase.h"

#include "ST_ComputeRPEmitter.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogComputeRPEmitter, Log, All);

UCLASS()
class ST_COMPUTERPEXAMPLE_API AST_ComputeRPEmitter : public AST_ComputeActorBase
{
	GENERATED_BODY()

public:
	AST_ComputeRPEmitter();

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
	virtual bool SetConstantParameters();
	virtual bool SetDynamicParameters();

	virtual FString GetOwnerName() const override;

protected:
	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Boid Parameters")
	FST_BoidCurrentParameters BoidCurrentParameters;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boids")
	TArray<FST_BoidItem> BoidsArray;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Niagara")
	UNiagaraComponent* Niagara = nullptr;

	FST_PingPongBuffer BoidsPingPongBuffer;
	FST_BoidsRDGStateData BoidsRDGStateData;

	virtual TSoftObjectPtr<UNiagaraSystem> GetNiagaraSystem() const;

private:
	FMatrix BoundsMatrix;
};

inline FString AST_ComputeRPEmitter::GetOwnerName() const
{
	return "ComputeRPEmitter";
}