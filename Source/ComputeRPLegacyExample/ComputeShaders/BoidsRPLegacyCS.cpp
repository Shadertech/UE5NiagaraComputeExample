#include "ComputeShaders/BoidsRPLegacyCS.h"
#include "Kismet/GameplayStatics.h"
#include "SceneRendering.h"
#include "ScenePrivate.h"

#define BoidsExample_ThreadsPerGroup 512

void FBoidsExampleUniformParameters::Bind(const FShaderParameterMap& ParameterMap)
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

void FBoidsExampleUniformParameters::SetParameters(FRHIBatchedShaderParameters& BatchedParameters, FBoidConstantParameters& BoidConstantParameters, FBoidVariableParameters& BoidVariableParameters, float _deltaTime)
{
	if (numBoids.IsBound())
	{
		SetShaderValue(BatchedParameters, numBoids, BoidConstantParameters.numBoids);
	}

	if (maxSpeed.IsBound())
	{
		SetShaderValue(BatchedParameters, maxSpeed, BoidVariableParameters.maxSpeed);
	}

	if (randSeed.IsBound())
	{
		SetShaderValue(BatchedParameters, randSeed, FMath::Rand() % (INT32_MAX + 1));
	}

	if (boundsMatrix.IsBound())
	{
		SetShaderValue(BatchedParameters, boundsMatrix, BoidVariableParameters.transformMatrix);
	}

	if (boundsInverseMatrix.IsBound())
	{
		SetShaderValue(BatchedParameters, boundsInverseMatrix, BoidVariableParameters.inverseTransformMatrix);
	}

	if (boundsRadius.IsBound())
	{
		SetShaderValue(BatchedParameters, boundsRadius, BoidVariableParameters.boundsRadius);
	}

	if (deltaTime.IsBound())
	{
		SetShaderValue(BatchedParameters, deltaTime, _deltaTime * BoidVariableParameters.simulationSpeed);
	}

	if (minSpeed.IsBound())
	{
		SetShaderValue(BatchedParameters, minSpeed, BoidVariableParameters.minSpeed());
	}

	if (turnSpeed.IsBound())
	{
		SetShaderValue(BatchedParameters, turnSpeed, BoidVariableParameters.turnSpeed());
	}

	if (minDistance.IsBound())
	{
		SetShaderValue(BatchedParameters, minDistance, BoidVariableParameters.minDistance);
	}

	if (minDistanceSq.IsBound())
	{
		SetShaderValue(BatchedParameters, minDistanceSq, BoidVariableParameters.minDistanceSq());
	}

	if (cohesionFactor.IsBound())
	{
		SetShaderValue(BatchedParameters, cohesionFactor, BoidVariableParameters.cohesionFactor);
	}

	if (separationFactor.IsBound())
	{
		SetShaderValue(BatchedParameters, separationFactor, BoidVariableParameters.separationFactor);
	}

	if (alignmentFactor.IsBound())
	{
		SetShaderValue(BatchedParameters, alignmentFactor, BoidVariableParameters.alignmentFactor);
	}
}

void FBoidsExampleBufferParameters::Bind(const FShaderParameterMap& ParameterMap)
{
	boidsOut.Bind(ParameterMap, TEXT("boidsOut"));
	boidsIn.Bind(ParameterMap, TEXT("boidsIn"));
}

void FBoidsExampleBufferParameters::SetParameters(FRHIBatchedShaderParameters& BatchedParameters, FShaderResourceViewRHIRef readRef, FUnorderedAccessViewRHIRef writeRef)
{
	SetUAVParameter(BatchedParameters, boidsOut, writeRef);
	SetSRVParameter(BatchedParameters, boidsIn, readRef);
}

void FBoidsExampleBufferParameters::UnsetParameters(FRHIBatchedShaderUnbinds& BatchedUnbinds)
{
	UnsetUAVParameter(BatchedUnbinds, boidsOut);
	UnsetSRVParameter(BatchedUnbinds, boidsIn);
}

FBoidsRPInitLegacyExampleCS::FBoidsRPInitLegacyExampleCS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
	: FGlobalShader(Initializer)
{
	boidsExampleUniformParameters.Bind(Initializer.ParameterMap);
	boidsExampleBufferParameters.Bind(Initializer.ParameterMap);
}

// ____________________________________________ Set Uniform Parameters

void FBoidsRPInitLegacyExampleCS::SetUniformParameters(FRHIBatchedShaderParameters& BatchedParameters, FBoidConstantParameters& BoidConstantParameters, FBoidVariableParameters& BoidVariableParameters, float _deltaTime)
{
	boidsExampleUniformParameters.SetParameters(BatchedParameters, BoidConstantParameters, BoidVariableParameters, _deltaTime);
}

// ____________________________________________ Set Buffer Parameters

void FBoidsRPInitLegacyExampleCS::SetBufferParameters(FRHIBatchedShaderParameters& BatchedParameters, FShaderResourceViewRHIRef readRef, FUnorderedAccessViewRHIRef writeRef)
{
	boidsExampleBufferParameters.SetParameters(BatchedParameters, readRef, writeRef);
}

// ____________________________________________ Unset Buffer Parameters

void FBoidsRPInitLegacyExampleCS::UnsetBufferParameters(FRHIBatchedShaderUnbinds& BatchedUnbinds)
{
	boidsExampleBufferParameters.UnsetParameters(BatchedUnbinds);
}

bool FBoidsRPInitLegacyExampleCS::ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
{
	return true;
}
void FBoidsRPInitLegacyExampleCS::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	OutEnvironment.CompilerFlags.Add(CFLAG_StandardOptimization);

	OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_X"), BoidsExample_ThreadsPerGroup);
	OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_Y"), 1);
	OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_Z"), 1);
}

FBoidsRPUpdateLegacyExampleCS::FBoidsRPUpdateLegacyExampleCS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
	: FGlobalShader(Initializer)
{
	boidsExampleUniformParameters.Bind(Initializer.ParameterMap);
	boidsExampleBufferParameters.Bind(Initializer.ParameterMap);
}

// ____________________________________________ Set Uniform Parameters

void FBoidsRPUpdateLegacyExampleCS::SetUniformParameters(FRHIBatchedShaderParameters& BatchedParameters, FBoidConstantParameters& BoidConstantParameters, FBoidVariableParameters& BoidVariableParameters, float _deltaTime)
{
	boidsExampleUniformParameters.SetParameters(BatchedParameters, BoidConstantParameters, BoidVariableParameters, _deltaTime);
}

// ____________________________________________ Set Buffer Parameters

void FBoidsRPUpdateLegacyExampleCS::SetBufferParameters(FRHIBatchedShaderParameters& BatchedParameters, FShaderResourceViewRHIRef readRef, FUnorderedAccessViewRHIRef writeRef)
{
	boidsExampleBufferParameters.SetParameters(BatchedParameters, readRef, writeRef);
}

// ____________________________________________ Unset Buffer Parameters

void FBoidsRPUpdateLegacyExampleCS::UnsetBufferParameters(FRHIBatchedShaderUnbinds& BatchedUnbinds)
{
	boidsExampleBufferParameters.UnsetParameters(BatchedUnbinds);
}

bool FBoidsRPUpdateLegacyExampleCS::ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
{
	return true;
}
void FBoidsRPUpdateLegacyExampleCS::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	OutEnvironment.CompilerFlags.Add(CFLAG_StandardOptimization);

	OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_X"), BoidsExample_ThreadsPerGroup);
	OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_Y"), 1);
	OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_Z"), 1);
}