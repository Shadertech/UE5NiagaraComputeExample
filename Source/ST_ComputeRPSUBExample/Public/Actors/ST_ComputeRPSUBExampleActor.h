// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/ST_ComputeRPEmitter.h"
#include "SceneView/ST_SceneUBtoMatSceneViewExtension.h"
#include "ST_ComputeRPSUBExampleActor.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogComputeRPSUBExampleActor, Log, All);

UCLASS()
class ST_COMPUTERPSUBEXAMPLE_API AST_ComputeRPSUBExampleActor : public AST_ComputeRPEmitter
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AST_ComputeRPSUBExampleActor();

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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Category = "ShaderTech|Material")
	UMaterialInstanceDynamic* MatiD = nullptr;

private:
	TSharedPtr<FST_SceneUBtoMatSceneViewExtension> ViewExtension = nullptr;

	FST_PingPongBuffer FlattenBoidsPingPongBuffer;
};

inline FString AST_ComputeRPSUBExampleActor::GetOwnerName() const
{
	return "ComputeRPSUBExampleActor";
}