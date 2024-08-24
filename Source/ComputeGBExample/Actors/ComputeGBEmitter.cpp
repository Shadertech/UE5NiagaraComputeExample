#include "ComputeGBEmitter.h"
#include "ComputeGBExample.h"
#include "ComputeShaders/BoidsCS.h"
#include "GlobalShader.h"
#include "Engine/World.h"
#include "Niagara/NiagaraDataInterfaceStructuredBuffer.h"
#include "Settings/ComputeExampleSettings.h"
#include "Niagara/NDIStructuredBufferFunctionLibrary.h"

DEFINE_LOG_CATEGORY(LogComputeGBEmitter);

AComputeGBEmitter::AComputeGBEmitter()
{
	Niagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara"));
	Niagara->SetupAttachment(RootComponent);
	Niagara->bAutoActivate = false;
}

void AComputeGBEmitter::BeginPlay()
{
	Super::BeginPlay();

	FComputeGBExampleModule::Get().Register(this);
}

void AComputeGBEmitter::BeginDestroy()
{
	if (Niagara != nullptr)
	{
		Niagara->DeactivateImmediate();
	}
	FComputeGBExampleModule::Get().Deregister(this);
	Super::BeginDestroy();
}

void AComputeGBEmitter::Tick(float DeltaTime)
{
	LastDeltaTime = DeltaTime;

	SetDynamicParameters();

	Super::Tick(DeltaTime);
}

void AComputeGBEmitter::InitComputeShader_GameThread()
{
	check(IsInGameThread());

	BoundsMatrix = FMatrix::Identity.ConcatTranslation(GetActorLocation());
	BoidsArray.Empty(); // Clear any existing elements in the array

	BoidsPingPongBuffer = FPingPongBuffer();
	BoidsRenderGraphPasses = FBoidsRenderGraphPasses();

	const UComputeExampleSettings* computeExampleSettings = UComputeExampleSettings::GetComputeExampleSettings();
	Niagara->SetAsset(computeExampleSettings->BoidsEmitterVFX.LoadSynchronous());

	if (SetConstantParameters() && SetDynamicParameters())
	{
		Niagara->Activate(true);
	}
}

void AComputeGBEmitter::ExecuteComputeShader_RenderThread(FRDGBuilder& GraphBuilder)
{
	check(IsInRenderingThread());

	BoidsRenderGraphPasses.ClearPasses();

	if (!BoidsRenderGraphPasses.init)
	{
		FComputeShader_Boids::InitBoidsExample_RenderThread(GraphBuilder, TEXT("ComputeGBEmitter_"), BoidsArray, BoidCurrentParameters, BoidsRenderGraphPasses, BoidsPingPongBuffer);
		return;
	}

	FComputeShader_Boids::DispatchBoidsExample_RenderThread(GraphBuilder, TEXT("ComputeGBEmitter_"), BoidCurrentParameters, BoidsRenderGraphPasses, BoidsPingPongBuffer, LastDeltaTime);
}

void AComputeGBEmitter::DisposeComputeShader_GameThread()
{
	check(IsInGameThread());
}

bool AComputeGBEmitter::SetConstantParameters()
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

bool AComputeGBEmitter::SetDynamicParameters()
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