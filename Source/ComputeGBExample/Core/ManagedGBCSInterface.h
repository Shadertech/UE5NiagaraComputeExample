#pragma once

#include "CoreMinimal.h"
#include "RenderGraphBuilder.h"
#include "UObject/Interface.h"
#include "ManagedGBCSInterface.generated.h"

UINTERFACE(MinimalAPI)
class UManagedGBCSInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class IManagedGBCSInterface
{
	GENERATED_BODY()

public:

	virtual void InitComputeShader_GameThread()
	{}

	virtual void ExecuteComputeShader_RenderThread(FRDGBuilder& GraphBuilder)
	{}

	virtual void DisposeComputeShader_GameThread()
	{}
};
