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
	virtual void InitComputeShader()
	{}

	virtual void InitComputeShader_RenderThread(FRHICommandListImmediate& RHICmdList)
	{}

	virtual void ExecuteComputeShader(float DeltaTime)
	{}

	virtual void ExecuteComputeShader_RenderThread(FRHICommandListImmediate& RHICmdList)
	{}
};
