// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#include "ComputeShaders/ST_BoidsRPLegacyCS.h"
#include "Kismet/GameplayStatics.h"
#include "SceneRendering.h"
#include "ScenePrivate.h"

#define BoidsExample_ThreadsPerGroup 512

void FST_BoidsExampleUniformParameters::Bind(const FShaderParameterMap& ParameterMap)
{
	numBoids.Bind(ParameterMap, TEXT("numBoids"));
	maxSpeed.Bind(ParameterMap, TEXT("maxSpeed"));
	randSeed.Bind(ParameterMap, TEXT("randSeed"));
	boundsMatrix.Bind(ParameterMap, TEXT("boundsMatrix"));
	boundsInverseMatrix.Bind(ParameterMap, TEXT("boundsInverseMatrix"));
	boundsRadius.Bind(ParameterMap, TEXT("boundsRadius"));

	deltaTime.Bind(ParameterMap, TEXT("deltaTime"));
	minSpeed.Bind(ParameterMap, TEXT("minSpeed"));
	turnSpeed.Bind(ParameterMap, TEXT("turnSpeed"));

	minDistance.Bind(ParameterMap, TEXT("minDistance"));
	minDistanceSq.Bind(ParameterMap, TEXT("minDistanceSq"));
	cohesionFactor.Bind(ParameterMap, TEXT("cohesionFactor"));
	separationFactor.Bind(ParameterMap, TEXT("separationFactor"));
	alignmentFactor.Bind(ParameterMap, TEXT("alignmentFactor"));
}

void FST_BoidsExampleUniformParameters::SetParameters(FRHIBatchedShaderParameters& BatchedParameters, FST_BoidCurrentParameters& BoidCurrentParameters, float _deltaTime)
{
	if (numBoids.IsBound())
	{
		SetShaderValue(BatchedParameters, numBoids, BoidCurrentParameters.ConstantParameters.numBoids);
	}

	if (maxSpeed.IsBound())
	{
		SetShaderValue(BatchedParameters, maxSpeed, BoidCurrentParameters.DynamicParameters.maxSpeed);
	}

	if (randSeed.IsBound())
	{
		SetShaderValue(BatchedParameters, randSeed, FMath::Rand() % (INT32_MAX + 1));
	}

	if (boundsMatrix.IsBound())
	{
		SetShaderValue(BatchedParameters, boundsMatrix, BoidCurrentParameters.transformMatrix);
	}

	if (boundsInverseMatrix.IsBound())
	{
		SetShaderValue(BatchedParameters, boundsInverseMatrix, BoidCurrentParameters.inverseTransformMatrix);
	}

	if (boundsRadius.IsBound())
	{
		SetShaderValue(BatchedParameters, boundsRadius, BoidCurrentParameters.boundsRadius);
	}

	if (deltaTime.IsBound())
	{
		SetShaderValue(BatchedParameters, deltaTime, _deltaTime * BoidCurrentParameters.DynamicParameters.simulationSpeed);
	}

	if (minSpeed.IsBound())
	{
		SetShaderValue(BatchedParameters, minSpeed, BoidCurrentParameters.DynamicParameters.minSpeed());
	}

	if (turnSpeed.IsBound())
	{
		SetShaderValue(BatchedParameters, turnSpeed, BoidCurrentParameters.DynamicParameters.turnSpeed());
	}

	if (minDistance.IsBound())
	{
		SetShaderValue(BatchedParameters, minDistance, BoidCurrentParameters.DynamicParameters.minDistance);
	}

	if (minDistanceSq.IsBound())
	{
		SetShaderValue(BatchedParameters, minDistanceSq, BoidCurrentParameters.DynamicParameters.minDistanceSq());
	}

	if (cohesionFactor.IsBound())
	{
		SetShaderValue(BatchedParameters, cohesionFactor, BoidCurrentParameters.DynamicParameters.cohesionFactor);
	}

	if (separationFactor.IsBound())
	{
		SetShaderValue(BatchedParameters, separationFactor, BoidCurrentParameters.DynamicParameters.separationFactor);
	}

	if (alignmentFactor.IsBound())
	{
		SetShaderValue(BatchedParameters, alignmentFactor, BoidCurrentParameters.DynamicParameters.alignmentFactor);
	}
}

void FST_BoidsExampleBufferParameters::Bind(const FShaderParameterMap& ParameterMap)
{
	boidsOut.Bind(ParameterMap, TEXT("boidsOut"));
	boidsIn.Bind(ParameterMap, TEXT("boidsIn"));
}

void FST_BoidsExampleBufferParameters::SetParameters(FRHIBatchedShaderParameters& BatchedParameters, FShaderResourceViewRHIRef readRef, FUnorderedAccessViewRHIRef writeRef)
{
	SetUAVParameter(BatchedParameters, boidsOut, writeRef);
	SetSRVParameter(BatchedParameters, boidsIn, readRef);
}

void FST_BoidsExampleBufferParameters::UnsetParameters(FRHIBatchedShaderUnbinds& BatchedUnbinds)
{
	UnsetUAVParameter(BatchedUnbinds, boidsOut);
	UnsetSRVParameter(BatchedUnbinds, boidsIn);
}

FST_BoidsRPInitLegacyExampleCS::FST_BoidsRPInitLegacyExampleCS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
	: FGlobalShader(Initializer)
{
	boidsExampleUniformParameters.Bind(Initializer.ParameterMap);
	boidsExampleBufferParameters.Bind(Initializer.ParameterMap);
}

// ____________________________________________ Set Uniform Parameters

void FST_BoidsRPInitLegacyExampleCS::SetUniformParameters(FRHIBatchedShaderParameters& BatchedParameters, FST_BoidCurrentParameters& BoidCurrentParameters, float _deltaTime)
{
	boidsExampleUniformParameters.SetParameters(BatchedParameters, BoidCurrentParameters, _deltaTime);
}

// ____________________________________________ Set Buffer Parameters

void FST_BoidsRPInitLegacyExampleCS::SetBufferParameters(FRHIBatchedShaderParameters& BatchedParameters, FShaderResourceViewRHIRef readRef, FUnorderedAccessViewRHIRef writeRef)
{
	boidsExampleBufferParameters.SetParameters(BatchedParameters, readRef, writeRef);
}

// ____________________________________________ Unset Buffer Parameters

void FST_BoidsRPInitLegacyExampleCS::UnsetBufferParameters(FRHIBatchedShaderUnbinds& BatchedUnbinds)
{
	boidsExampleBufferParameters.UnsetParameters(BatchedUnbinds);
}

// This will tell the engine to create the shader and where the shader entry point is.
//                            ShaderType            ShaderPath           Shader function name Type
IMPLEMENT_GLOBAL_SHADER(FST_BoidsRPInitLegacyExampleCS, "/ComputeCore/Private/ComputeRPExample/CS_Boids.usf", "GenerateBoids", SF_Compute);

bool FST_BoidsRPInitLegacyExampleCS::ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
{
	return true;
}
void FST_BoidsRPInitLegacyExampleCS::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	OutEnvironment.CompilerFlags.Add(CFLAG_StandardOptimization);

	OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_X"), BoidsExample_ThreadsPerGroup);
	OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_Y"), 1);
	OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_Z"), 1);
}

FST_BoidsRPUpdateLegacyExampleCS::FST_BoidsRPUpdateLegacyExampleCS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
	: FGlobalShader(Initializer)
{
	boidsExampleUniformParameters.Bind(Initializer.ParameterMap);
	boidsExampleBufferParameters.Bind(Initializer.ParameterMap);
}

// ____________________________________________ Set Uniform Parameters

void FST_BoidsRPUpdateLegacyExampleCS::SetUniformParameters(FRHIBatchedShaderParameters& BatchedParameters, FST_BoidCurrentParameters& BoidCurrentParameters, float _deltaTime)
{
	boidsExampleUniformParameters.SetParameters(BatchedParameters, BoidCurrentParameters, _deltaTime);
}

// ____________________________________________ Set Buffer Parameters

void FST_BoidsRPUpdateLegacyExampleCS::SetBufferParameters(FRHIBatchedShaderParameters& BatchedParameters, FShaderResourceViewRHIRef readRef, FUnorderedAccessViewRHIRef writeRef)
{
	boidsExampleBufferParameters.SetParameters(BatchedParameters, readRef, writeRef);
}

// ____________________________________________ Unset Buffer Parameters

void FST_BoidsRPUpdateLegacyExampleCS::UnsetBufferParameters(FRHIBatchedShaderUnbinds& BatchedUnbinds)
{
	boidsExampleBufferParameters.UnsetParameters(BatchedUnbinds);
}

// This will tell the engine to create the shader and where the shader entry point is.
//                            ShaderType            ShaderPath           Shader function name Type
IMPLEMENT_GLOBAL_SHADER(FST_BoidsRPUpdateLegacyExampleCS, "/ComputeCore/Private/ComputeRPExample/CS_Boids.usf", "UpdateBoids", SF_Compute);

bool FST_BoidsRPUpdateLegacyExampleCS::ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
{
	return true;
}
void FST_BoidsRPUpdateLegacyExampleCS::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	OutEnvironment.CompilerFlags.Add(CFLAG_StandardOptimization);

	OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_X"), BoidsExample_ThreadsPerGroup);
	OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_Y"), 1);
	OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_Z"), 1);
}