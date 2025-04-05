// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/ComputeRPEmitter.h"
#include "SceneView/SceneUBtoMatSceneViewExtension.h"
#include "ComputeRPSUBExampleActor.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogComputeRPSUBExampleActor, Log, All);

UCLASS()
class COMPUTERPSUBEXAMPLE_API AComputeRPSUBExampleActor : public AComputeRPEmitter
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AComputeRPSUBExampleActor();

protected:
	// Start Managed Compute Shader Interface 
	virtual void InitComputeShader_GameThread() override;
	virtual void InitComputeShader_RenderThread(FRHICommandListImmediate& RHICmdList) override;

	virtual void ExecuteComputeShader_GameThread(float DeltaTime) override;
	virtual void ExecuteComputeShader_RenderThread(FRHICommandListImmediate& RHICmdList) override;

	virtual void DisposeComputeShader_GameThread() override;
	virtual void DisposeComputeShader_RenderThread(FRHICommandListImmediate& RHICmdList) override;
	// End Managed Compute Shader Interface

	virtual bool SetConstantParameters() override;
	virtual bool SetDynamicParameters() override;
	virtual FString GetOwnerName() const override;

	virtual TSoftObjectPtr<UNiagaraSystem> GetNiagaraSystem() const override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Category = "Material")
	UMaterialInstanceDynamic* MatiD = nullptr;

private:
	TSharedPtr<FSceneUBtoMatSceneViewExtension> ViewExtension = nullptr;

	FPingPongBuffer FlattenBoidsPingPongBuffer;
};

inline FString AComputeRPSUBExampleActor::GetOwnerName() const
{
	return "ComputeRPSUBExampleActor";
}