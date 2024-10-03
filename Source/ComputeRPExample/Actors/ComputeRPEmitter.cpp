#include "ComputeRPEmitter.h"
#include "GraphBuilder/BoidsGB.h"
#include "Settings/ComputeExampleSettings.h"
#include "Niagara/NDIStructuredBufferFunctionLibrary.h"

DEFINE_LOG_CATEGORY(LogComputeRPEmitter);

// ____________________________________________ Constructor

AComputeRPEmitter::AComputeRPEmitter()
{
	Niagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara"));
	Niagara->SetupAttachment(RootComponent);
	Niagara->bAutoActivate = false;
}

// ____________________________________________ BeginPlay

void AComputeRPEmitter::BeginPlay()
{
	Super::BeginPlay();
}

// ____________________________________________ BeginDestroy

void AComputeRPEmitter::BeginDestroy()
{
	if (Niagara != nullptr)
	{
		Niagara->DeactivateImmediate();
	}
	Super::BeginDestroy();
}

// ____________________________________________ DisposeComputeShader
void AComputeRPEmitter::DisposeComputeShader_GameThread()
{
	check(IsInGameThread());
	Super::DisposeComputeShader_GameThread();
}

void AComputeRPEmitter::DisposeComputeShader_RenderThread(FRHICommandListImmediate& RHICmdList)
{
	check(IsInRenderingThread());

	Super::DisposeComputeShader_RenderThread(RHICmdList);

	BoidsPingPongBuffer.Dispose();
}

// ____________________________________________ Init Compute Shader

void AComputeRPEmitter::InitComputeShader_GameThread()
{
	check(IsInGameThread());

	Super::InitComputeShader_GameThread();

	BoundsMatrix = FMatrix::Identity.ConcatTranslation(GetActorLocation());
	BoidsArray.Empty(); // Clear any existing elements in the array

	BoidsPingPongBuffer = FPingPongBuffer();

	Niagara->SetAsset(GetNiagaraSystem().LoadSynchronous());

	if (SetConstantParameters() && SetDynamicParameters())
	{
		Niagara->Activate(true);
	}
}

void AComputeRPEmitter::InitComputeShader_RenderThread(FRHICommandListImmediate& RHICmdList)
{
	check(IsInRenderingThread());

	Super::InitComputeShader_RenderThread(RHICmdList);

	FRDGBuilder GraphBuilder(RHICmdList);

	BoidsRenderGraphPasses.ClearPasses();

	FGraphBullder_Boids::InitBoidsExample_RenderThread(GraphBuilder, *GetOwnerName(), BoidsArray, BoidCurrentParameters, BoidsRenderGraphPasses, BoidsPingPongBuffer);

	GraphBuilder.Execute();
}

// ____________________________________________ Execute Compute Shader

void AComputeRPEmitter::ExecuteComputeShader_GameThread(float DeltaTime)
{
	check(IsInGameThread());

	Super::ExecuteComputeShader_GameThread(DeltaTime);

	SetDynamicParameters();
}

void AComputeRPEmitter::ExecuteComputeShader_RenderThread(FRHICommandListImmediate& RHICmdList)
{
	check(IsInRenderingThread());

	Super::ExecuteComputeShader_RenderThread(RHICmdList);

	FRDGBuilder GraphBuilder(RHICmdList);

	BoidsRenderGraphPasses.ClearPasses();

	FGraphBullder_Boids::ExecuteBoidsExample_RenderThread(GraphBuilder, *GetOwnerName(), BoidCurrentParameters, BoidsRenderGraphPasses, BoidsPingPongBuffer, LastDeltaTime);

	GraphBuilder.Execute();
}

TSoftObjectPtr<UNiagaraSystem> AComputeRPEmitter::GetNiagaraSystem() const
{
	const UComputeExampleSettings* computeExampleSettings = UComputeExampleSettings::GetComputeExampleSettings();
	return computeExampleSettings->BoidsEmitterVFX;
}

bool AComputeRPEmitter::SetConstantParameters()
{
	const UComputeExampleSettings* computeExampleSettings = UComputeExampleSettings::GetComputeExampleSettings();
	BoidCurrentParameters.ConstantParameters = computeExampleSettings->BoidConstantParameters;

	// Resize the array to accommodate numboids elements
	BoidsArray.SetNum(BoidCurrentParameters.ConstantParameters.numBoids);

	if (Niagara == nullptr || Niagara->GetAsset() == nullptr)
	{
		return false;
	}

	UNDIStructuredBufferFunctionLibrary::SetNiagaraStructuredBuffer(Niagara, "boidsIn", BoidCurrentParameters.ConstantParameters.numBoids, nullptr);

	Niagara->SetIntParameter("numBoids", BoidCurrentParameters.ConstantParameters.numBoids);

	return true;
}

bool AComputeRPEmitter::SetDynamicParameters()
{
	const UComputeExampleSettings* computeExampleSettings = UComputeExampleSettings::GetComputeExampleSettings();
	BoidCurrentParameters.DynamicParameters = computeExampleSettings->BoidDynamicParameters;

	BoidCurrentParameters.boundsRadius = BoundsConstantParameters.Radius;
	BoidCurrentParameters.transformMatrix = (FMatrix44f)BoundsMatrix;
	BoidCurrentParameters.inverseTransformMatrix = (FMatrix44f)BoundsMatrix.Inverse();

	if (Niagara == nullptr || Niagara->GetAsset() == nullptr)
	{
		return false;
	}

	UNDIStructuredBufferFunctionLibrary::SetNiagaraStructuredBuffer(Niagara, "boidsIn", BoidCurrentParameters.ConstantParameters.numBoids, BoidsPingPongBuffer.ReadPooled);

	Niagara->SetFloatParameter("meshScale", BoidCurrentParameters.DynamicParameters.meshScale);
	Niagara->SetFloatParameter("worldScale", BoidCurrentParameters.worldScale);

	return true;
}