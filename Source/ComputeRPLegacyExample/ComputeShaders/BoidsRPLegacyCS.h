#pragma once

#include "CoreMinimal.h"
#include "Data/BoidsLib.h"
#include "GlobalShader.h"
#include "ShaderParameters.h"
#include "DataDrivenShaderPlatformInfo.h"

#include "Core/ManagedRPCSInterface.h"
#include "RHICommandList.h"
#include "Utils/PingPongBuffer.h"

class FBoidsExampleUniformParameters
{
	DECLARE_INLINE_TYPE_LAYOUT(FBoidsExampleUniformParameters, NonVirtual);

public:
	void Bind(const FShaderParameterMap& ParameterMap);

	template<typename TParamRef>
	void SetParameters(FRHICommandList& RHICmdList, const TParamRef& ShaderRHI, FBoidConstantParameters& BoidConstantParameters, FBoidVariableParameters& BoidVariableParameters, float _deltaTime);

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

	template<typename TParamRef>
	void SetParameters(FRHICommandList& RHICmdList, const TParamRef& ShaderRHI, FShaderResourceViewRHIRef readRef, FUnorderedAccessViewRHIRef writeRef);

	template<typename TParamRef>
	void UnsetParameters(FRHICommandList& RHICmdList, const TParamRef& ShaderRHI);

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

class FInitBoidsLegacyExampleCS : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FInitBoidsLegacyExampleCS, Global);

public:
	FInitBoidsLegacyExampleCS() {}

	explicit FInitBoidsLegacyExampleCS(const ShaderMetaType::CompiledShaderInitializerType& Initializer);

	void SetUniformParameters(FRHICommandList& RHICmdList, FRHIComputeShader* ShaderRHI, FBoidConstantParameters& BoidConstantParameters, FBoidVariableParameters& BoidVariableParameters, float _deltaTime);
	void SetBufferParameters(FRHICommandList& RHICmdList, FRHIComputeShader* ShaderRHI, FShaderResourceViewRHIRef readRef, FUnorderedAccessViewRHIRef writeRef);
	void UnsetBufferParameters(FRHICommandList& RHICmdList, FRHIComputeShader* ShaderRHI);

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters);
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);

private:
	LAYOUT_FIELD(FBoidsExampleUniformParameters, boidsExampleUniformParameters);
	LAYOUT_FIELD(FBoidsExampleBufferParameters, boidsExampleBufferParameters);
};

// This will tell the engine to create the shader and where the shader entry point is.
//                            ShaderType            ShaderPath           Shader function name Type
IMPLEMENT_GLOBAL_SHADER(FInitBoidsLegacyExampleCS, "/ComputeExample/CS_Boids.usf", "GenerateBoids", SF_Compute);

class FBoidsUpdateLegacyExampleCS : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FBoidsUpdateLegacyExampleCS, Global);

public:
	FBoidsUpdateLegacyExampleCS() {}
	explicit FBoidsUpdateLegacyExampleCS(const ShaderMetaType::CompiledShaderInitializerType& Initializer);

	void SetUniformParameters(FRHICommandList& RHICmdList, FRHIComputeShader* ShaderRHI, FBoidConstantParameters& BoidConstantParameters, FBoidVariableParameters& BoidVariableParameters, float _deltaTime);
	void SetBufferParameters(FRHICommandList& RHICmdList, FRHIComputeShader* ShaderRHI, FShaderResourceViewRHIRef readRef, FUnorderedAccessViewRHIRef writeRef);
	void UnsetBufferParameters(FRHICommandList& RHICmdList, FRHIComputeShader* ShaderRHI);

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters);
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);

private:
	LAYOUT_FIELD(FBoidsExampleUniformParameters, boidsExampleUniformParameters);
	LAYOUT_FIELD(FBoidsExampleBufferParameters, boidsExampleBufferParameters);
};

// This will tell the engine to create the shader and where the shader entry point is.
//                            ShaderType            ShaderPath           Shader function name Type
IMPLEMENT_GLOBAL_SHADER(FBoidsUpdateLegacyExampleCS, "/ComputeExample/CS_Boids.usf", "UpdateBoids", SF_Compute);