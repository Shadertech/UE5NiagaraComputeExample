// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ST_ComputeRPExample.h"
#include "GlobalShader.h"
#include "DataDrivenShaderPlatformInfo.h"
#include "ShaderParameterStruct.h"

/************************************************************************/
/* Simple static vertex buffer.                                         */
/************************************************************************/
class FST_SimpleScreenVertexBuffer : public FVertexBuffer
{
public:
	/** Initialize the RHI for this rendering resource */
	virtual void InitRHI(FRHICommandListBase& RHICmdList) override;
};

/************************************************************************/
/* A simple passthrough vertexshader that we will use.                  */
/************************************************************************/
class FST_SimplePassThroughVS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FST_SimplePassThroughVS);
	SHADER_USE_PARAMETER_STRUCT(FST_SimplePassThroughVS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
	// Add your own VS params here!
	END_SHADER_PARAMETER_STRUCT()

public:
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters);
};