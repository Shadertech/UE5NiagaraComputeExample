#include "NiagaraBoids.h"
#include "Kismet/GameplayStatics.h"
#include "SceneRendering.h"
#include "ScenePrivate.h"
#include "NiagaraDataInterfaceArrayFloat.h"

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

	SetNiagaraConstantParameters();
}

void ANiagaraBoids::Tick(float DeltaTime)
{
	SetNiagaraConstantParameters();
	SetNiagaraVariableParameters();

	Super::Tick(DeltaTime);
}

// ____________________________________________ BeginDestroy

void ANiagaraBoids::BeginDestroy()
{
	Super::BeginDestroy();
}

bool ANiagaraBoids::SetNiagaraConstantParameters()
{
	if (Niagara == nullptr || Niagara->GetAsset() == nullptr)
	{
		return false;
	}

	Niagara->SetIntParameter("numBoids", BoidConstantParameters.numBoids);
	return true;
}

void ANiagaraBoids::SetNiagaraVariableParameters()
{
	if (Niagara == nullptr || Niagara->GetAsset() == nullptr)
	{
		return;
	}

	BoundsMatrix = FMatrix::Identity.ConcatTranslation(GetActorLocation());

	FNiagaraUserRedirectionParameterStore& ParameterStore = Niagara->GetOverrideParameters();
	FNiagaraVariable SBParameter = FNiagaraVariable(FNiagaraTypeDefinition(UNiagaraDataInterfaceArrayMatrix::StaticClass()), TEXT("boundsMatrixArr"));
	UNiagaraDataInterfaceArrayMatrix* data = (UNiagaraDataInterfaceArrayMatrix*)ParameterStore.GetDataInterface(SBParameter);
	TArray<FMatrix44f> MatrixArray;
	MatrixArray.Add((FMatrix44f)BoundsMatrix);
	MatrixArray.Add((FMatrix44f)BoundsMatrix.Inverse());
	data->InternalMatrixData = MatrixArray;

	Niagara->SetFloatParameter("boundsRadius", BoundsConstantParameters.Radius);

	Niagara->SetFloatParameter("meshScale", BoidVariableParameters.meshScale);
	Niagara->SetFloatParameter("worldScale", BoidVariableParameters.worldScale);
}