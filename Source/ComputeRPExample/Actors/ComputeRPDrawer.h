#pragma once

#include "CoreMinimal.h"
#include "Base/ComputeActorBase.h"
#include "Data/BoidsLib.h"
#include "Utils/PingPongBuffer.h"
#include "Engine/TextureRenderTarget2D.h"
#include "NiagaraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "ComputeRPDrawer.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogComputeRPDrawer, Log, All);

UCLASS()
class COMPUTERPEXAMPLE_API AComputeRPDrawer : public AComputeActorBase
{
	GENERATED_BODY()
	
public:
	AComputeRPDrawer();

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
	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Niagara")
	UNiagaraComponent* Niagara = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Plane")
	UStaticMeshComponent* Plane = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid Parameters")
	FBoidConstantParameters BoidConstantParameters;
	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Boid Parameters")
	FBoidCurrentParameters BoidCurrentParameters;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boids")
	TArray<FBoidItem> BoidsArray;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Boids")
	UTextureRenderTarget2D* RenderTarget = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Plane")
	UMaterialInstanceDynamic* mid = nullptr;

private:
	FPingPongBuffer BoidsPingPongBuffer;
	FRDGTextureRef RenderTargetTexture;
	FMatrix BoundsMatrix;
	FBoidsRDGStateData BoidsRDGStateData;

private:
	void CreateRenderTexture();
};

inline FString AComputeRPDrawer::GetOwnerName() const
{
	return "ComputeRPDrawer";
}