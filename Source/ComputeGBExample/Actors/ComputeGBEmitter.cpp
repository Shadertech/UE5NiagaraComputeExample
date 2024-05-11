#include "ComputeGBEmitter.h"
#include "Kismet/GameplayStatics.h"
#include "SceneRendering.h"
#include "ScenePrivate.h"
#include "ComputeGBExample.h"
#include "ComputeShaders/BoidsCS.h"
#include "GlobalShader.h"
#include "Engine/World.h"
#include "Niagara/NiagaraDataInterfaceStructuredBuffer.h"

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

	SetNiagaraVariableParameters();

	Super::Tick(DeltaTime);
}

bool AComputeGBEmitter::SetNiagaraConstantParameters()
{
	if (Niagara == nullptr || Niagara->GetAsset() == nullptr)
	{
		return false;
	}

	// Get the parameter store of the Niagara component
	FNiagaraUserRedirectionParameterStore& ParameterStore = Niagara->GetOverrideParameters();
	FNiagaraVariable SBParameter = FNiagaraVariable(FNiagaraTypeDefinition(UNiagaraDataInterfaceStructuredBuffer::StaticClass()), TEXT("boidsIn"));
	UNiagaraDataInterfaceStructuredBuffer* data = (UNiagaraDataInterfaceStructuredBuffer*)ParameterStore.GetDataInterface(SBParameter);

	if (data)
	{
		data->numBoids = BoidConstantParameters.numBoids;
		data->SetBuffer(nullptr);
	}

	Niagara->SetIntParameter("numBoids", BoidConstantParameters.numBoids);

	return true;
}

void AComputeGBEmitter::SetNiagaraVariableParameters()
{
	if (Niagara == nullptr || Niagara->GetAsset() == nullptr)
	{
		return;
	}

	// Get the parameter store of the Niagara component
	FNiagaraUserRedirectionParameterStore& ParameterStore = Niagara->GetOverrideParameters();
	FNiagaraVariable SBParameter = FNiagaraVariable(FNiagaraTypeDefinition(UNiagaraDataInterfaceStructuredBuffer::StaticClass()), TEXT("boidsIn"));
	UNiagaraDataInterfaceStructuredBuffer* data = (UNiagaraDataInterfaceStructuredBuffer*)ParameterStore.GetDataInterface(SBParameter);

	if (data)
	{
		data->numBoids = BoidConstantParameters.numBoids;

		if (!data->GetBuffer().IsValid() 
			&& BoidsPingPongBuffer.ReadPooled.IsValid())
		{
			data->SetBuffer(BoidsPingPongBuffer.ReadPooled);
		}
	}

	Niagara->SetFloatParameter("meshScale", BoidVariableParameters.meshScale);
	Niagara->SetFloatParameter("worldScale", BoidVariableParameters.worldScale);
}

void AComputeGBEmitter::InitComputeShader()
{
	check(IsInGameThread());

	BoundsMatrix = FMatrix::Identity.ConcatTranslation(GetActorLocation());
	BoidsArray.Empty(); // Clear any existing elements in the array

	// Resize the array to accommodate numboids elements
	BoidsArray.SetNum(BoidConstantParameters.numBoids);

	BoidsPingPongBuffer = FPingPongBuffer();
	BoidsRenderGraphPasses = FBoidsRenderGraphPasses();

	if (SetNiagaraConstantParameters())
	{
		Niagara->Activate(true);
	}
}

void AComputeGBEmitter::ExecuteComputeShader_RenderThread(FRDGBuilder& GraphBuilder)
{
	check(IsInRenderingThread());

	BoidsRenderGraphPasses.ClearPasses();

	BoidVariableParameters.boundsRadius = BoundsConstantParameters.Radius;
	BoidVariableParameters.transformMatrix = (FMatrix44f)BoundsMatrix;
	BoidVariableParameters.inverseTransformMatrix = (FMatrix44f)BoundsMatrix.Inverse();

	if (!BoidsRenderGraphPasses.init)
	{
		FComputeShader_Boids::InitBoidsExample_RenderThread(GraphBuilder, TEXT("ComputeGBEmitter_"), BoidsArray, BoidConstantParameters, BoidVariableParameters, BoidsRenderGraphPasses, BoidsPingPongBuffer);
	}

	FComputeShader_Boids::DispatchBoidsExample_RenderThread(GraphBuilder, TEXT("ComputeGBEmitter_"), BoidConstantParameters, BoidVariableParameters, BoidsRenderGraphPasses, BoidsPingPongBuffer, LastDeltaTime);
}

void AComputeGBEmitter::DisposeComputeShader()
{
	check(IsInGameThread());
}