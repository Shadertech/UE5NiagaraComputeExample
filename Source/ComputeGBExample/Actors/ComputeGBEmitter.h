#pragma once

#include "CoreMinimal.h"
#include "Base/ComputeActorBase.h"
#include "Data/BoidsLib.h"
#include "Utils/PingPongBuffer.h"
#include "Core/ManagedGBCSInterface.h"
#include "NiagaraComponent.h"
#include "ComputeGBEmitter.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogComputeGBEmitter, Log, All);

UCLASS()
class COMPUTEGBEXAMPLE_API AComputeGBEmitter : public AComputeActorBase, public IManagedGBCSInterface
{
	GENERATED_BODY()

public:
	AComputeGBEmitter();

protected:
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

protected:
	bool SetConstantParameters();
	bool SetDynamicParameters();

public:
	virtual void Tick(float DeltaTime) override;

	// Managed Compute Shader Interface 
	virtual void InitComputeShader_GameThread() override;
	virtual void ExecuteComputeShader_RenderThread(FRDGBuilder& GraphBuilder) override;
	virtual void DisposeComputeShader_GameThread() override;

protected:
	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Boid Parameters")
	FBoidCurrentParameters BoidCurrentParameters;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boids")
	TArray<FBoidItem> BoidsArray;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Niagara")
	UNiagaraComponent* Niagara = nullptr;
private:
	FPingPongBuffer BoidsPingPongBuffer;

	FMatrix BoundsMatrix;
	FBoidsRenderGraphPasses BoidsRenderGraphPasses;

	float LastDeltaTime = 0.0f;
};
