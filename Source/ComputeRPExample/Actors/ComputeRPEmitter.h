#pragma once

#include "CoreMinimal.h"
#include "Data/BoidsLib.h"
#include "Utils/PingPongBuffer.h"
#include "GlobalShader.h"
#include "NiagaraComponent.h"
#include "DataDrivenShaderPlatformInfo.h"

#include "Base/ComputeActorBase.h"
#include "Core/ManagedRPCSInterface.h"
#include "RPCSManager.h"
#include "RHICommandList.h"

#include "ComputeRPEmitter.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogComputeRPEmitter, Log, All);

UCLASS()
class COMPUTERPEXAMPLE_API AComputeRPEmitter : public AComputeActorBase, public IManagedRPCSInterface
{
	GENERATED_BODY()

public:
	AComputeRPEmitter();

protected:
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	virtual void DisposeComputeShader_GameThread() override;

protected:
	bool SetConstantParameters();
	bool SetDynamicParameters();

public:
	// Managed Compute Shader Interface 
	virtual void InitComputeShader_GameThread() override;
	virtual void InitComputeShader_RenderThread(FRHICommandListImmediate& RHICmdList) override;

	virtual void ExecuteComputeShader_GameThread(float DeltaTime) override;
	virtual void ExecuteComputeShader_RenderThread(FRHICommandListImmediate& RHICmdList) override;

protected:
	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Boid Parameters")
	FBoidCurrentParameters BoidCurrentParameters;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boids")
	TArray<FBoidItem> BoidsArray;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Niagara")
	UNiagaraComponent* Niagara = nullptr;

private:
	UPROPERTY(Transient)
	ARPCSManager* cachedRPCSManager = nullptr;

	FPingPongBuffer BoidsPingPongBuffer;

	FMatrix BoundsMatrix;

	float LastDeltaTime = 0.0f;
};