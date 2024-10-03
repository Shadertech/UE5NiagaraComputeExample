#pragma once

#include "CoreMinimal.h"
#include "Data/BoidsLib.h"
#include "Utils/PingPongBuffer.h"
#include "GlobalShader.h"
#include "NiagaraComponent.h"
#include "DataDrivenShaderPlatformInfo.h"

#include "Base/ComputeActorBase.h"

#include "ComputeRPEmitter.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogComputeRPEmitter, Log, All);

UCLASS()
class COMPUTERPEXAMPLE_API AComputeRPEmitter : public AComputeActorBase
{
	GENERATED_BODY()

public:
	AComputeRPEmitter();

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
	FBoidCurrentParameters BoidCurrentParameters;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boids")
	TArray<FBoidItem> BoidsArray;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Niagara")
	UNiagaraComponent* Niagara = nullptr;

	FPingPongBuffer BoidsPingPongBuffer;
	FBoidsRenderGraphPasses BoidsRenderGraphPasses;

	virtual TSoftObjectPtr<UNiagaraSystem> GetNiagaraSystem() const;

private:
	FMatrix BoundsMatrix;
};

inline FString AComputeRPEmitter::GetOwnerName() const
{
	return "ComputeRPEmitter";
}