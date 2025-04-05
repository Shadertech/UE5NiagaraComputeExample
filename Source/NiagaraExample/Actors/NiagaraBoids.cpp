// Copyright (c) 2025 Aaron Trotter (ShaderTech). All Rights Reserved.

#include "NiagaraBoids.h"
#include "Kismet/GameplayStatics.h"
#include "SceneRendering.h"
#include "ScenePrivate.h"
#include "NiagaraDataInterfaceArrayFloat.h"
#include "Settings/ComputeExampleSettings.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "NiagaraFunctionLibrary.h"

//#include "NiagaraDataInterfaceArrayImpl.h"
//#include "NiagaraDataInterfaceArrayImplHelpers.h"
//NDIARRAY_GENERATE_IMPL_LWC(UNiagaraDataInterfaceArrayMatrix, FMatrix44f, MatrixData)

DEFINE_LOG_CATEGORY(LogNiagaraBoids);

// ____________________________________________ Constructor

ANiagaraBoids::ANiagaraBoids()
{
	Niagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara"));
	Niagara->SetupAttachment(RootComponent);
}

// ____________________________________________ BeginPlay

void ANiagaraBoids::BeginPlay()
{
	Super::BeginPlay();

	const UComputeExampleSettings* computeExampleSettings = UComputeExampleSettings::GetComputeExampleSettings();
	Niagara->SetAsset(computeExampleSettings->BoidsVFX.LoadSynchronous());

	if (SetConstantParameters() && SetDynamicParameters())
	{
		Niagara->Activate(true);
	}
}

void ANiagaraBoids::Tick(float DeltaTime)
{
	SetConstantParameters();
	SetDynamicParameters();

	Super::Tick(DeltaTime);
}

// ____________________________________________ BeginDestroy

void ANiagaraBoids::BeginDestroy()
{
	Super::BeginDestroy();
}

bool ANiagaraBoids::SetConstantParameters()
{
	if (Niagara == nullptr || Niagara->GetAsset() == nullptr)
	{
		return false;
	}

	const UComputeExampleSettings* computeExampleSettings = UComputeExampleSettings::GetComputeExampleSettings();
	BoidCurrentParameters.ConstantParameters = computeExampleSettings->BoidConstantParameters;

	Niagara->SetIntParameter("numBoids", BoidCurrentParameters.ConstantParameters.numBoids);
	return true;
}

bool ANiagaraBoids::SetDynamicParameters()
{
	if (Niagara == nullptr || Niagara->GetAsset() == nullptr)
	{
		return false;
	}

	const UComputeExampleSettings* computeExampleSettings = UComputeExampleSettings::GetComputeExampleSettings();
	BoidCurrentParameters.DynamicParameters = computeExampleSettings->BoidDynamicParameters;

	BoundsMatrix = FMatrix::Identity.ConcatTranslation(GetActorLocation());

	TArray<FMatrix> MatrixArray;
	MatrixArray.Add(BoundsMatrix);
	MatrixArray.Add(BoundsMatrix.Inverse());

	bool EditorAndNotPlaying = false;

#if WITH_EDITOR
	UWorld* World = Niagara->GetWorld();
	if (World && !World->IsGameWorld())
	{
		EditorAndNotPlaying = true;
	}
#endif

	if (EditorAndNotPlaying)
	{
		/*if (UNiagaraDataInterfaceArrayMatrix* ArrayDI = UNiagaraFunctionLibrary::GetDataInterface<UNiagaraDataInterfaceArrayMatrix>(Niagara, "boundsMatrixArr"))
		{
			auto* ArrayProxy = static_cast<typename UNiagaraDataInterfaceArrayMatrix::FProxyType*>(ArrayDI->GetProxy());
			const TArray<FMatrix> TempMatrixArray = MatrixArray;
			ArrayProxy->SetArrayData(TempMatrixArray);
		}*/
		FNiagaraUserRedirectionParameterStore& ParameterStore = Niagara->GetOverrideParameters();
		FNiagaraVariable SBParameter = FNiagaraVariable(FNiagaraTypeDefinition(UNiagaraDataInterfaceArrayMatrix::StaticClass()), TEXT("boundsMatrixArr"));
		UNiagaraDataInterfaceArrayMatrix* data = (UNiagaraDataInterfaceArrayMatrix*)ParameterStore.GetDataInterface(SBParameter);
		data->InternalMatrixData = (TArray<FMatrix44f>)MatrixArray;
	}
	else
	{
		UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayMatrix(Niagara, "boundsMatrixArr", MatrixArray);
	}

	Niagara->SetFloatParameter("boundsRadius", BoundsConstantParameters.Radius);

	Niagara->SetFloatParameter("meshScale", BoidCurrentParameters.DynamicParameters.meshScale);
	Niagara->SetFloatParameter("worldScale", BoidCurrentParameters.worldScale);

	Niagara->SetFloatParameter("simulationSpeed", BoidCurrentParameters.DynamicParameters.simulationSpeed);
	Niagara->SetFloatParameter("minSpeed", BoidCurrentParameters.DynamicParameters.minSpeed());
	Niagara->SetFloatParameter("maxSpeed", BoidCurrentParameters.DynamicParameters.maxSpeed);
	Niagara->SetFloatParameter("turnSpeed", BoidCurrentParameters.DynamicParameters.turnSpeed());
	Niagara->SetFloatParameter("minDistance", BoidCurrentParameters.DynamicParameters.minDistance);
	Niagara->SetFloatParameter("minDistanceSq", BoidCurrentParameters.DynamicParameters.minDistanceSq());
	Niagara->SetFloatParameter("cohesionFactor", BoidCurrentParameters.DynamicParameters.cohesionFactor);
	Niagara->SetFloatParameter("separationFactor", BoidCurrentParameters.DynamicParameters.separationFactor);
	Niagara->SetFloatParameter("alignmentFactor", BoidCurrentParameters.DynamicParameters.alignmentFactor);
	return true;
}