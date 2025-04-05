// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RHICommandList.h"
#include "ManagedRPCSInterface.generated.h"

UINTERFACE(MinimalAPI)
class UManagedRPCSInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class IManagedRPCSInterface
{
	GENERATED_BODY()

public:
	virtual void InitComputeShader_GameThread()
	{}

	virtual void InitComputeShader_RenderThread(FRHICommandListImmediate& RHICmdList)
	{}

	virtual void ExecuteComputeShader_GameThread(float DeltaTime)
	{}

	virtual void ExecuteComputeShader_RenderThread(FRHICommandListImmediate& RHICmdList)
	{}

	virtual void DisposeComputeShader_GameThread()
	{}

	virtual void DisposeComputeShader_RenderThread(FRHICommandListImmediate& RHICmdList)
	{}
};