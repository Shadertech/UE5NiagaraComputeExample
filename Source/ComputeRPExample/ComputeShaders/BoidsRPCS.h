#pragma once

#include "CoreMinimal.h"
#include "Data/BoidsLib.h"
#include "GlobalShader.h"
#include "DataDrivenShaderPlatformInfo.h"

#include "Core/ManagedRPCSInterface.h"
#include "RHICommandList.h"

class FInitBoidsExampleCS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FInitBoidsExampleCS);
	SHADER_USE_PARAMETER_STRUCT(FInitBoidsExampleCS, FGlobalShader);

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
	static inline void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);
};

// This will tell the engine to create the shader and where the shader entry point is.
//                            ShaderType            ShaderPath           Shader function name Type
IMPLEMENT_GLOBAL_SHADER(FInitBoidsExampleCS, "/ComputeExample/CS_Boids.usf", "GenerateBoids", SF_Compute);

class FBoidsUpdateExampleCS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FBoidsUpdateExampleCS);
	SHADER_USE_PARAMETER_STRUCT(FBoidsUpdateExampleCS, FGlobalShader);

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
	static inline void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);
};

// This will tell the engine to create the shader and where the shader entry point is.
//                            ShaderType            ShaderPath           Shader function name Type
IMPLEMENT_GLOBAL_SHADER(FBoidsUpdateExampleCS, "/ComputeExample/CS_Boids.usf", "UpdateBoids", SF_Compute);

class FBoidsCopyBufferExampleCS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FBoidsCopyBufferExampleCS);
	SHADER_USE_PARAMETER_STRUCT(FBoidsCopyBufferExampleCS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<FBoidItem>, boidsIn)
		SHADER_PARAMETER_RDG_BUFFER_UAV(RWStructuredBuffer<FBoidItem>, boidsOut)
		SHADER_PARAMETER(uint32, numBoids)
	END_SHADER_PARAMETER_STRUCT()

public:
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters);
	static inline void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);
};

// This will tell the engine to create the shader and where the shader entry point is.
//                            ShaderType            ShaderPath           Shader function name Type
IMPLEMENT_GLOBAL_SHADER(FBoidsCopyBufferExampleCS, "/ComputeExample/CS_BoidsCopyBuffer.usf", "CopyBuffer", SF_Compute);