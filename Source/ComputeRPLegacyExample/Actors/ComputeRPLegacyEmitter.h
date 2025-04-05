// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/BoidsLib.h"
#include "Utils/PingPongBuffer.h"
#include "NiagaraComponent.h"

#include "Base/ComputeActorBase.h"
#include "RHICommandList.h"
#include "RHIResources.h"

#include "ComputeRPLegacyEmitter.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogComputeRPLegacyEmitter, Log, All);

UCLASS()
class COMPUTERPLEGACYEXAMPLE_API AComputeRPLegacyEmitter : public AComputeActorBase
{
	GENERATED_BODY()

public:
	AComputeRPLegacyEmitter();

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
	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Boid Parameters")
	FBoidCurrentParameters BoidCurrentParameters;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boids")
	TArray<FBoidItem> BoidsArray;

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

inline FString AComputeRPLegacyEmitter::GetOwnerName() const
{
	return "ComputeRPLegacyEmitter";
}