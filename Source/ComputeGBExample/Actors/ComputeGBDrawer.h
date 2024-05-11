#pragma once

#include "CoreMinimal.h"
#include "Base/ComputeActorBase.h"
#include "Data/BoidsLib.h"
#include "Utils/PingPongBuffer.h"
#include "Core/ManagedGBCSInterface.h"
#include "Engine/TextureRenderTarget2D.h"
#include "NiagaraComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "ComputeGBDrawer.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogComputeGBDrawer, Log, All);

UCLASS()
class COMPUTEGBEXAMPLE_API AComputeGBDrawer : public AComputeActorBase, public IManagedGBCSInterface
{
	GENERATED_BODY()
	
public:
	AComputeGBDrawer();

protected:
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

public:
	virtual void Tick(float DeltaTime) override;

	// Managed Compute Shader Interface
	virtual void InitComputeShader() override;
	virtual void ExecuteComputeShader_RenderThread(FRDGBuilder& GraphBuilder) override;
	virtual void DisposeComputeShader() override;

protected:
	UFUNCTION(BlueprintCallable, Category = "Niagara")
	bool SetNiagaraConstantParameters();
	UFUNCTION(BlueprintCallable, Category = "Niagara")
	void SetNiagaraVariableParameters();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Niagara")
	UNiagaraComponent* Niagara = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Plane")
	UStaticMeshComponent* Plane = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid Parameters")
	FBoidConstantParameters BoidConstantParameters;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid Parameters")
	FBoidVariableParameters BoidVariableParameters;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boids")
	TArray<FBoidItem> BoidsArray;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "Niagara")
	UTextureRenderTarget2D* RenderTarget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Plane")
	UMaterialInstance* material = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Plane")
	UMaterialInstanceDynamic* mid = nullptr;

private:
	FPingPongBuffer BoidsPingPongBuffer;
	FRDGTextureRef RenderTargetTexture;
	FMatrix BoundsMatrix;
	FBoidsRenderGraphPasses BoidsRenderGraphPasses;
	float LastDeltaTime = 0.0f;

private:
	void CreateRenderTexture();
};