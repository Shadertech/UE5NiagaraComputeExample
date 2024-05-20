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

template<typename TParamRef>
void FBoidsExampleUniformParameters::SetParameters(FRHICommandList& RHICmdList, const TParamRef& ShaderRHI, FBoidConstantParameters& BoidConstantParameters, FBoidVariableParameters& BoidVariableParameters, float _deltaTime)
{
	if (numBoids.IsBound())
	{
		SetShaderValue(RHICmdList, ShaderRHI, numBoids, BoidConstantParameters.numBoids);
	}

	if (maxSpeed.IsBound())
	{
		SetShaderValue(RHICmdList, ShaderRHI, maxSpeed, BoidVariableParameters.maxSpeed);
	}

	if (randSeed.IsBound())
	{
		SetShaderValue(RHICmdList, ShaderRHI, randSeed, FMath::Rand() % (INT32_MAX + 1));
	}

	if (boundsMatrix.IsBound())
	{
		SetShaderValue(RHICmdList, ShaderRHI, boundsMatrix, BoidVariableParameters.transformMatrix);
	}

	if (boundsInverseMatrix.IsBound())
	{
		SetShaderValue(RHICmdList, ShaderRHI, boundsInverseMatrix, BoidVariableParameters.inverseTransformMatrix);
	}

	if (boundsRadius.IsBound())
	{
		SetShaderValue(RHICmdList, ShaderRHI, boundsRadius, BoidVariableParameters.boundsRadius);
	}

	if (deltaTime.IsBound())
	{
		SetShaderValue(RHICmdList, ShaderRHI, deltaTime, _deltaTime * BoidVariableParameters.simulationSpeed);
	}

	if (minSpeed.IsBound())
	{
		SetShaderValue(RHICmdList, ShaderRHI, minSpeed, BoidVariableParameters.minSpeed());
	}

	if (turnSpeed.IsBound())
	{
		SetShaderValue(RHICmdList, ShaderRHI, turnSpeed, BoidVariableParameters.turnSpeed());
	}

	if (minDistance.IsBound())
	{
		SetShaderValue(RHICmdList, ShaderRHI, minDistance, BoidVariableParameters.minDistance);
	}

	if (minDistanceSq.IsBound())
	{
		SetShaderValue(RHICmdList, ShaderRHI, minDistanceSq, BoidVariableParameters.minDistanceSq());
	}

	if (cohesionFactor.IsBound())
	{
		SetShaderValue(RHICmdList, ShaderRHI, cohesionFactor, BoidVariableParameters.cohesionFactor);
	}

	if (separationFactor.IsBound())
	{
		SetShaderValue(RHICmdList, ShaderRHI, separationFactor, BoidVariableParameters.separationFactor);
	}

	if (alignmentFactor.IsBound())
	{
		SetShaderValue(RHICmdList, ShaderRHI, alignmentFactor, BoidVariableParameters.alignmentFactor);
	}
}

void FBoidsExampleBufferParameters::Bind(const FShaderParameterMap& ParameterMap)
{
	boidsOut.Bind(ParameterMap, TEXT("boidsOut"));
	boidsIn.Bind(ParameterMap, TEXT("boidsIn"));
}

template<typename TParamRef>
void FBoidsExampleBufferParameters::SetParameters(FRHICommandList& RHICmdList, const TParamRef& ShaderRHI, FShaderResourceViewRHIRef readRef, FUnorderedAccessViewRHIRef writeRef)
{
	if (boidsOut.IsBound())
	{
		RHICmdList.SetUAVParameter(ShaderRHI, boidsOut.GetBaseIndex(), writeRef);
	}

	if (boidsIn.IsBound())
	{
		RHICmdList.SetShaderResourceViewParameter(ShaderRHI, boidsIn.GetBaseIndex(), readRef);
	}
}

template<typename TParamRef>
void FBoidsExampleBufferParameters::UnsetParameters(FRHICommandList& RHICmdList, const TParamRef& ShaderRHI)
{
	if (boidsOut.IsBound())
	{
		RHICmdList.SetUAVParameter(ShaderRHI, boidsOut.GetBaseIndex(), nullptr);
	}

	if (boidsIn.IsBound())
	{
		RHICmdList.SetShaderResourceViewParameter(ShaderRHI, boidsIn.GetBaseIndex(), nullptr);
	}
}

FInitBoidsLegacyExampleCS::FInitBoidsLegacyExampleCS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
	: FGlobalShader(Initializer)
{
	boidsExampleUniformParameters.Bind(Initializer.ParameterMap);
	boidsExampleBufferParameters.Bind(Initializer.ParameterMap);
}

// ____________________________________________ Set Uniform Parameters

void FInitBoidsLegacyExampleCS::SetUniformParameters(FRHICommandList& RHICmdList, FRHIComputeShader* ShaderRHI, FBoidConstantParameters& BoidConstantParameters, FBoidVariableParameters& BoidVariableParameters, float _deltaTime)
{
	boidsExampleUniformParameters.SetParameters(RHICmdList, ShaderRHI, BoidConstantParameters, BoidVariableParameters, _deltaTime);
}

// ____________________________________________ Set Buffer Parameters

void FInitBoidsLegacyExampleCS::SetBufferParameters(FRHICommandList& RHICmdList, FRHIComputeShader* ShaderRHI, FShaderResourceViewRHIRef readRef, FUnorderedAccessViewRHIRef writeRef)
{
	boidsExampleBufferParameters.SetParameters(RHICmdList, ShaderRHI, readRef, writeRef);
}

// ____________________________________________ Unset Buffer Parameters

void FInitBoidsLegacyExampleCS::UnsetBufferParameters(FRHICommandList& RHICmdList, FRHIComputeShader* ShaderRHI)
{
	boidsExampleBufferParameters.UnsetParameters(RHICmdList, ShaderRHI);
}

bool FInitBoidsLegacyExampleCS::ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
{
	return true;
}
void FInitBoidsLegacyExampleCS::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	OutEnvironment.CompilerFlags.Add(CFLAG_StandardOptimization);

	OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_X"), BoidsExample_ThreadsPerGroup);
	OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_Y"), 1);
	OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_Z"), 1);
}

FBoidsUpdateLegacyExampleCS::FBoidsUpdateLegacyExampleCS(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
	: FGlobalShader(Initializer)
{
	boidsExampleUniformParameters.Bind(Initializer.ParameterMap);
	boidsExampleBufferParameters.Bind(Initializer.ParameterMap);
}

// ____________________________________________ Set Uniform Parameters

void FBoidsUpdateLegacyExampleCS::SetUniformParameters(FRHICommandList& RHICmdList, FRHIComputeShader* ShaderRHI, FBoidConstantParameters& BoidConstantParameters, FBoidVariableParameters& BoidVariableParameters, float _deltaTime)
{
	boidsExampleUniformParameters.SetParameters(RHICmdList, ShaderRHI, BoidConstantParameters, BoidVariableParameters, _deltaTime);
}

// ____________________________________________ Set Buffer Parameters

void FBoidsUpdateLegacyExampleCS::SetBufferParameters(FRHICommandList& RHICmdList, FRHIComputeShader* ShaderRHI, FShaderResourceViewRHIRef readRef, FUnorderedAccessViewRHIRef writeRef)
{
	boidsExampleBufferParameters.SetParameters(RHICmdList, ShaderRHI, readRef, writeRef);
}

// ____________________________________________ Unset Buffer Parameters

void FBoidsUpdateLegacyExampleCS::UnsetBufferParameters(FRHICommandList& RHICmdList, FRHIComputeShader* ShaderRHI)
{
	boidsExampleBufferParameters.UnsetParameters(RHICmdList, ShaderRHI);
}

bool FBoidsUpdateLegacyExampleCS::ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
{
	return true;
}
void FBoidsUpdateLegacyExampleCS::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);
	OutEnvironment.CompilerFlags.Add(CFLAG_StandardOptimization);

	OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_X"), BoidsExample_ThreadsPerGroup);
	OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_Y"), 1);
	OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_Z"), 1);
}