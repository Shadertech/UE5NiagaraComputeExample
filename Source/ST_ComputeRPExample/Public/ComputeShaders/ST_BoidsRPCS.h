// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#pragma once

#include "Data/ST_BoidsLib.h"
#include "GlobalShader.h"
#include "ShaderParameterStruct.h"

class FST_BoidsRPInitExampleCS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FST_BoidsRPInitExampleCS);
	SHADER_USE_PARAMETER_STRUCT(FST_BoidsRPInitExampleCS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_RDG_BUFFER_UAV(RWStructuredBuffer<FST_BoidItem>, boidsOut)
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

class FST_BoidsRPUpdateExampleCS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FST_BoidsRPUpdateExampleCS);
	SHADER_USE_PARAMETER_STRUCT(FST_BoidsRPUpdateExampleCS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<FST_BoidItem>, boidsIn)
		SHADER_PARAMETER_RDG_BUFFER_UAV(RWStructuredBuffer<FST_BoidItem>, boidsOut)
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

class FST_BoidsRPCopyBufferExampleCS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FST_BoidsRPCopyBufferExampleCS);
	SHADER_USE_PARAMETER_STRUCT(FST_BoidsRPCopyBufferExampleCS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<FST_BoidItem>, boidsIn)
		SHADER_PARAMETER_RDG_BUFFER_UAV(RWStructuredBuffer<FST_BoidItem>, boidsOut)
		SHADER_PARAMETER(uint32, numBoids)
	END_SHADER_PARAMETER_STRUCT()

public:
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters);
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);
};