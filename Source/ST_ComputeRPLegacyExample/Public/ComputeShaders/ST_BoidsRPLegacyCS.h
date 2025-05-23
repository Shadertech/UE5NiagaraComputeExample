// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Data/ST_BoidsLib.h"
#include "GlobalShader.h"
#include "ShaderParameters.h"
#include "ShaderParameterStruct.h"
#include "DataDrivenShaderPlatformInfo.h"

#include "RHICommandList.h"
#include "Utils/ST_PingPongBuffer.h"

class FST_BoidsExampleUniformParameters
{
	DECLARE_INLINE_TYPE_LAYOUT(FST_BoidsExampleUniformParameters, NonVirtual);

public:
	void Bind(const FShaderParameterMap& ParameterMap);

	void SetParameters(FRHIBatchedShaderParameters& BatchedParameters, FST_BoidCurrentParameters& BoidCurrentParameters, float _deltaTime);

	friend FArchive& operator<<(FArchive& Ar, FST_BoidsExampleUniformParameters& P)
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

class FST_BoidsExampleBufferParameters
{
	DECLARE_INLINE_TYPE_LAYOUT(FST_BoidsExampleBufferParameters, NonVirtual);

public:
	void Bind(const FShaderParameterMap& ParameterMap);

	void SetParameters(FRHIBatchedShaderParameters& BatchedParameters, FShaderResourceViewRHIRef readRef, FUnorderedAccessViewRHIRef writeRef);

	void UnsetParameters(FRHIBatchedShaderUnbinds& BatchedUnbinds);

	friend FArchive& operator<<(FArchive& Ar, FST_BoidsExampleBufferParameters& P)
	{
		Ar << P.boidsOut;
		Ar << P.boidsIn;
		return Ar;
	}

private:
	LAYOUT_FIELD(FShaderResourceParameter, boidsOut);
	LAYOUT_FIELD(FShaderResourceParameter, boidsIn);
};

class FST_BoidsRPInitLegacyExampleCS : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FST_BoidsRPInitLegacyExampleCS, Global);

public:
	FST_BoidsRPInitLegacyExampleCS() {}

	explicit FST_BoidsRPInitLegacyExampleCS(const ShaderMetaType::CompiledShaderInitializerType& Initializer);

	void SetUniformParameters(FRHIBatchedShaderParameters& BatchedParameters, FST_BoidCurrentParameters& BoidCurrentParameters, float _deltaTime);
	void SetBufferParameters(FRHIBatchedShaderParameters& BatchedParameters, FShaderResourceViewRHIRef readRef, FUnorderedAccessViewRHIRef writeRef);
	void UnsetBufferParameters(FRHIBatchedShaderUnbinds& BatchedUnbinds);

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters);
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);

private:
	LAYOUT_FIELD(FST_BoidsExampleUniformParameters, boidsExampleUniformParameters);
	LAYOUT_FIELD(FST_BoidsExampleBufferParameters, boidsExampleBufferParameters);
};

class FST_BoidsRPUpdateLegacyExampleCS : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FST_BoidsRPUpdateLegacyExampleCS, Global);

public:
	FST_BoidsRPUpdateLegacyExampleCS() {}
	explicit FST_BoidsRPUpdateLegacyExampleCS(const ShaderMetaType::CompiledShaderInitializerType& Initializer);

	void SetUniformParameters(FRHIBatchedShaderParameters& BatchedParameters, FST_BoidCurrentParameters& BoidCurrentParameters, float _deltaTime);
	void SetBufferParameters(FRHIBatchedShaderParameters& BatchedParameters, FShaderResourceViewRHIRef readRef, FUnorderedAccessViewRHIRef writeRef);
	void UnsetBufferParameters(FRHIBatchedShaderUnbinds& BatchedUnbinds);

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters);
	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);

private:
	LAYOUT_FIELD(FST_BoidsExampleUniformParameters, boidsExampleUniformParameters);
	LAYOUT_FIELD(FST_BoidsExampleBufferParameters, boidsExampleBufferParameters);
};