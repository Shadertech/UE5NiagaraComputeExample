#pragma once

#include "CoreMinimal.h"
#include "Data/BoidsLib.h"
#include "GlobalShader.h"
#include "ShaderParameters.h"
#include "ShaderParameterStruct.h"
#include "DataDrivenShaderPlatformInfo.h"

#include "Core/ManagedRPCSInterface.h"
#include "RHICommandList.h"
#include "Utils/PingPongBuffer.h"

class FBoidsExampleUniformParameters
{
	DECLARE_INLINE_TYPE_LAYOUT(FBoidsExampleUniformParameters, NonVirtual);

public:
	void Bind(const FShaderParameterMap& ParameterMap);

	void SetParameters(FRHIBatchedShaderParameters& BatchedParameters, FBoidConstantParameters& BoidConstantParameters, FBoidVariableParameters& BoidVariableParameters, float _deltaTime);

	friend FArchive& operator<<(FArchive& Ar, FBoidsExampleUniformParameters& P)
	{
		Ar << P.numBoids;
		Ar << P.maxSpeed;
		Ar << P.randSeed;
		Ar << P.boundsMatrix;
		Ar << P.boundsInverseMatrix;
		Ar << P.boundsRadius;
		return Ar;
	}

private:
	LAYOUT_FIELD(FShaderParameter, numBoids);
	LAYOUT_FIELD(FShaderParameter, maxSpeed);
	LAYOUT_FIELD(FShaderParameter, randSeed);
	LAYOUT_FIELD(FShaderParameter, boundsMatrix);
	LAYOUT_FIELD(FShaderParameter, boundsInverseMatrix);
	LAYOUT_FIELD(FShaderParameter, boundsRadius);

	//UpdateOnly
	LAYOUT_FIELD(FShaderParameter, deltaTime);
	LAYOUT_FIELD(FShaderParameter, minSpeed);
	LAYOUT_FIELD(FShaderParameter, turnSpeed);

	LAYOUT_FIELD(FShaderParameter, minDistance);
	LAYOUT_FIELD(FShaderParameter, minDistanceSq);
	LAYOUT_FIELD(FShaderParameter, cohesionFactor);
	LAYOUT_FIELD(FShaderParameter, separationFactor);
	LAYOUT_FIELD(FShaderParameter, alignmentFactor);
};

class FBoidsExampleBufferParameters
{
	DECLARE_INLINE_TYPE_LAYOUT(FBoidsExampleBufferParameters, NonVirtual);

public:
	void Bind(const FShaderParameterMap& ParameterMap);

	void SetParameters(FRHIBatchedShaderParameters& BatchedParameters, FShaderResourceViewRHIRef readRef, FUnorderedAccessViewRHIRef writeRef);

	void UnsetParameters(FRHIBatchedShaderUnbinds& BatchedUnbinds);

	friend FArchive& operator<<(FArchive& Ar, FBoidsExampleBufferParameters& P)
	{
		Ar << P.boidsOut;
		Ar << P.boidsIn;
		return Ar;
	}

private:
	LAYOUT_FIELD(FShaderResourceParameter, boidsOut);
	LAYOUT_FIELD(FShaderResourceParameter, boidsIn);
};

class FBoidsRPInitLegacyExampleCS : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FBoidsRPInitLegacyExampleCS, Global);

public:
	FBoidsRPInitLegacyExampleCS() {}

	explicit FBoidsRPInitLegacyExampleCS(const ShaderMetaType::CompiledShaderInitializerType& Initializer);

	void SetUniformParameters(FRHIBatchedShaderParameters& BatchedParameters, FBoidConstantParameters& BoidConstantParameters, FBoidVariableParameters& BoidVariableParameters, float _deltaTime);
	void SetBufferParameters(FRHIBatchedShaderParameters& BatchedParameters, FShaderResourceViewRHIRef readRef, FUnorderedAccessViewRHIRef writeRef);
	void UnsetBufferParameters(FRHIBatchedShaderUnbinds& BatchedUnbinds);

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters);
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);

private:
	LAYOUT_FIELD(FBoidsExampleUniformParameters, boidsExampleUniformParameters);
	LAYOUT_FIELD(FBoidsExampleBufferParameters, boidsExampleBufferParameters);
};

// This will tell the engine to create the shader and where the shader entry point is.
//                            ShaderType            ShaderPath           Shader function name Type
IMPLEMENT_GLOBAL_SHADER(FBoidsRPInitLegacyExampleCS, "/ComputeExample/CS_Boids.usf", "GenerateBoids", SF_Compute);

class FBoidsRPUpdateLegacyExampleCS : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FBoidsRPUpdateLegacyExampleCS, Global);

public:
	FBoidsRPUpdateLegacyExampleCS() {}
	explicit FBoidsRPUpdateLegacyExampleCS(const ShaderMetaType::CompiledShaderInitializerType& Initializer);

	void SetUniformParameters(FRHIBatchedShaderParameters& BatchedParameters, FBoidConstantParameters& BoidConstantParameters, FBoidVariableParameters& BoidVariableParameters, float _deltaTime);
	void SetBufferParameters(FRHIBatchedShaderParameters& BatchedParameters, FShaderResourceViewRHIRef readRef, FUnorderedAccessViewRHIRef writeRef);
	void UnsetBufferParameters(FRHIBatchedShaderUnbinds& BatchedUnbinds);

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters);
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);

private:
	LAYOUT_FIELD(FBoidsExampleUniformParameters, boidsExampleUniformParameters);
	LAYOUT_FIELD(FBoidsExampleBufferParameters, boidsExampleBufferParameters);
};

// This will tell the engine to create the shader and where the shader entry point is.
//                            ShaderType            ShaderPath           Shader function name Type
IMPLEMENT_GLOBAL_SHADER(FBoidsRPUpdateLegacyExampleCS, "/ComputeExample/CS_Boids.usf", "UpdateBoids", SF_Compute);