// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Base/ST_ComputeActorBase.h"
#include "Data/ST_BoidsLib.h"
#include "Utils/ST_PingPongBuffer.h"
#include "Engine/TextureRenderTarget2D.h"
#include "NiagaraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "ST_ComputeRPDrawer.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogComputeRPDrawer, Log, All);

UCLASS()
class ST_COMPUTERPEXAMPLE_API AST_ComputeRPDrawer : public AST_ComputeActorBase
{
	GENERATED_BODY()
	
public:
	AST_ComputeRPDrawer();

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
	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "ShaderTech|BoidParameters")
	FST_BoidCurrentParameters BoidCurrentParameters;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ShaderTech|Boids")
	TArray<FST_BoidItem> BoidsArray;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, Category = "ShaderTech|Boids")
	UTextureRenderTarget2D* RenderTarget = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ShaderTech|Plane")
	UMaterialInstanceDynamic* MID = nullptr;

private:
	FST_PingPongBuffer BoidsPingPongBuffer;
	FRDGTextureRef RenderTargetTexture;
	FMatrix BoundsMatrix;
	FST_BoidsRDGStateData BoidsRDGStateData;

private:
	void CreateRenderTexture();
};

inline FString AST_ComputeRPDrawer::GetOwnerName() const
{
	return "ComputeRPDrawer";
}