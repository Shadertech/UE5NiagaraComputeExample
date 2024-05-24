#pragma once

#include "CoreMinimal.h"
#include "Data/BoidsLib.h"
#include "Utils/PingPongBuffer.h"
#include "ComputeGBExample.h"
#include "GlobalShader.h"
#include "ShaderParameters.h"
#include "ShaderParameterStruct.h"
#include "DataDrivenShaderPlatformInfo.h"

class FBoidsGBInitExampleCS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FBoidsGBInitExampleCS);
	SHADER_USE_PARAMETER_STRUCT(FBoidsGBInitExampleCS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_RDG_BUFFER_UAV(RWStructuredBuffer<FBoidItem>, boidsOut)
		SHADER_PARAMETER(uint32, numBoids)
		SHADER_PARAMETER(float, maxSpeed)
		SHADER_PARAMETER(uint32, randSeed)

		SHADER_PARAMETER(FMatrix44f, boundsMatrix)
		SHADER_PARAMETER(FMatrix44f, boundsInverseMatrix)
		SHADER_PARAMETER(float, boundsRadius)
	END_SHADER_PARAMETER_STRUCT()

public:
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters);
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);
};

class FBoidsGBUpdateExampleCS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FBoidsGBUpdateExampleCS);
	SHADER_USE_PARAMETER_STRUCT(FBoidsGBUpdateExampleCS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<FBoidItem>, boidsIn)
		SHADER_PARAMETER_RDG_BUFFER_UAV(RWStructuredBuffer<FBoidItem>, boidsOut)
		SHADER_PARAMETER(uint32, numBoids)
		SHADER_PARAMETER(float, deltaTime)

		SHADER_PARAMETER(float, minSpeed)
		SHADER_PARAMETER(float, maxSpeed)
		SHADER_PARAMETER(float, turnSpeed)

		SHADER_PARAMETER(float, minDistance)
		SHADER_PARAMETER(float, minDistanceSq)
		SHADER_PARAMETER(float, cohesionFactor)
		SHADER_PARAMETER(float, separationFactor)
		SHADER_PARAMETER(float, alignmentFactor)

		SHADER_PARAMETER(FMatrix44f, boundsMatrix)
		SHADER_PARAMETER(FMatrix44f, boundsInverseMatrix)
		SHADER_PARAMETER(float, boundsRadius)
	END_SHADER_PARAMETER_STRUCT()

public:
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters);
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);
};

class FComputeShader_Boids
{
public:
	static void InitBoidsExample_RenderThread(FRDGBuilder& GraphBuilder, const TCHAR* OwnerName, const TArray<FBoidItem>& BoidsArray,  const FBoidCurrentParameters& BoidCurrentParameters, FBoidsRenderGraphPasses& BoidsRenderGraphPasses, FPingPongBuffer& BoidsPingPongBuffer);
	static void GetBoidsExample_RenderThread(FRDGBuilder& GraphBuilder, FBoidsGPUDispatches& BoidsGPUDispatches);
	static void DispatchBoidsExample_RenderThread(FRDGBuilder& GraphBuilder, const TCHAR* OwnerName,  const FBoidCurrentParameters& BoidCurrentParameters, FBoidsRenderGraphPasses& BoidsRenderGraphPasses, FPingPongBuffer& BoidsPingPongBuffer, float DeltaTime);
	static void DispatchBoidsReadbackExample_RenderThread(FRDGBuilder& GraphBuilder, const TCHAR* OwnerName,  const FBoidCurrentParameters& BoidCurrentParameters, FBoidsRenderGraphPasses& BoidsRenderGraphPasses, FPingPongBuffer& BoidsPingPongBuffer, float DeltaTime, FBoidsGPUReadback& BoidsGPURequest);
};