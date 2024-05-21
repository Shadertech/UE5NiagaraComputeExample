#include "ComputeRPLegacyEmitter.h"
#include "Kismet/GameplayStatics.h"
#include "SceneRendering.h"
#include "ScenePrivate.h"
#include "Niagara/NiagaraDataInterfaceStructuredBufferLegacy.h"
#include "ComputeShaders/BoidsRPLegacyCS.h"

#define BoidsExample_ThreadsPerGroup 512
DEFINE_LOG_CATEGORY(LogComputeRPLegacyEmitter);

// ____________________________________________ Constructor

AComputeRPLegacyEmitter::AComputeRPLegacyEmitter()
{
	Niagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara"));
	Niagara->SetupAttachment(RootComponent);
	Niagara->bAutoActivate = false;
}

// ____________________________________________ BeginPlay

void AComputeRPLegacyEmitter::BeginPlay()
{
	Super::BeginPlay();

	cachedRPCSManager = ARPCSManager::Get(GetWorld());

	if (cachedRPCSManager != nullptr)
	{
		cachedRPCSManager->Register(this);
	}
}

// ____________________________________________ BeginDestroy

void AComputeRPLegacyEmitter::BeginDestroy()
{
	if (Niagara != nullptr)
	{
		Niagara->DeactivateImmediate();
	}
	Super::BeginDestroy();
	if (cachedRPCSManager != nullptr)
	{
		cachedRPCSManager->Deregister(this);
	}
}

// ____________________________________________ Init Compute Shader

void AComputeRPLegacyEmitter::InitComputeShader()
{
	check(IsInGameThread());

	BoidVariableParameters.boundsRadius = BoundsConstantParameters.Radius;
	BoidVariableParameters.transformMatrix = (FMatrix44f)BoundsMatrix;
	BoidVariableParameters.inverseTransformMatrix = (FMatrix44f)BoundsMatrix.Inverse();

	BoundsMatrix = FMatrix::Identity.ConcatTranslation(GetActorLocation());
	BoidsArray.Empty(); // Clear any existing elements in the array

	// Resize the array to accommodate numboids elements
	BoidsArray.SetNum(BoidConstantParameters.numBoids);

	if (SetNiagaraConstantParameters())
	{
		Niagara->Activate(true);
	}
}

void AComputeRPLegacyEmitter::InitComputeShader_RenderThread(FRHICommandListImmediate& RHICmdList)
{
	check(IsInRenderingThread());

	TShaderMapRef<FBoidsRPInitLegacyExampleCS> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	FRHIComputeShader* ShaderRHI = ComputeShader.GetComputeShader();
	SetComputePipelineState(RHICmdList, ShaderRHI);

	BoidItemSize = sizeof(FBoidItem);
	TResourceArray<FBoidItem> BoidItemRA;
	BoidItemRA.SetNum(BoidsArray.Num());

	// Copying data from BoidsArray to BoidItemRA
	FMemory::Memcpy(BoidItemRA.GetData(), BoidsArray.GetData(), BoidItemSize * BoidsArray.Num());

	// Creating a structured buffer for reading
	FRHIResourceCreateInfo CreateReadInfo(TEXT("BoidsInBuffer"));
	CreateReadInfo.ResourceArray = &BoidItemRA;
	readBuffer = RHICmdList.CreateStructuredBuffer(BoidItemSize, BoidItemSize * BoidsArray.Num(), BUF_StructuredBuffer | BUF_ShaderResource, CreateReadInfo);

	// Creating a structured buffer for writing
	FRHIResourceCreateInfo CreateWriteInfo(TEXT("BoidsOutBuffer"));
	CreateWriteInfo.ResourceArray = &BoidItemRA;
	writeBuffer = RHICmdList.CreateStructuredBuffer(BoidItemSize, BoidItemSize * BoidsArray.Num(), BUF_UnorderedAccess | BUF_ShaderResource, CreateWriteInfo);

	auto readSRVCreateDesc = FRHIViewDesc::CreateBufferSRV()
		.SetType(FRHIViewDesc::EBufferType::Structured)
		.SetNumElements(BoidConstantParameters.numBoids)
		.SetStride(BoidItemSize);

	readRef = RHICmdList.CreateShaderResourceView(readBuffer, readSRVCreateDesc);
	writeRef = RHICmdList.CreateUnorderedAccessView(writeBuffer, false, false);

	FRHIBatchedShaderParameters& BatchedParameters = RHICmdList.GetScratchShaderParameters();

	ComputeShader->SetUniformParameters(BatchedParameters, BoidConstantParameters, BoidVariableParameters, 0.0f);
	ComputeShader->SetBufferParameters(BatchedParameters, nullptr, writeRef);

	RHICmdList.SetBatchedShaderParameters(ShaderRHI, BatchedParameters);

	FIntVector GroupCounts = FIntVector(FMath::DivideAndRoundUp(BoidConstantParameters.numBoids, BoidsExample_ThreadsPerGroup), 1, 1);
	DispatchComputeShader(RHICmdList, ComputeShader, GroupCounts.X, GroupCounts.Y, GroupCounts.Z);

	FRHIBatchedShaderUnbinds& BatchedUnbinds = RHICmdList.GetScratchShaderUnbinds();
	/*UnsetShaderUAVs<FRHICommandList, FBoidsRPInitLegacyExampleCS>(RHICmdList, ComputeShader, ShaderRHI);
	UnsetShaderSRVs<FRHICommandList, FBoidsRPInitLegacyExampleCS>(RHICmdList, ComputeShader, ShaderRHI);*/
	ComputeShader->UnsetBufferParameters(BatchedUnbinds);
	RHICmdList.SetBatchedShaderUnbinds(ShaderRHI, BatchedUnbinds);

	RHICmdList.CopyBufferRegion(readBuffer, 0, writeBuffer, 0, BoidItemSize * BoidsArray.Num());

	readRef = RHICmdList.CreateShaderResourceView(readBuffer, readSRVCreateDesc);
}

// ____________________________________________ Execute Compute Shader

void AComputeRPLegacyEmitter::ExecuteComputeShader(float DeltaTime)
{
	check(IsInGameThread());

	LastDeltaTime = DeltaTime;

	BoidVariableParameters.boundsRadius = BoundsConstantParameters.Radius;
	BoidVariableParameters.transformMatrix = (FMatrix44f)BoundsMatrix;
	BoidVariableParameters.inverseTransformMatrix = (FMatrix44f)BoundsMatrix.Inverse();

	SetNiagaraVariableParameters();
}

void AComputeRPLegacyEmitter::ExecuteComputeShader_RenderThread(FRHICommandListImmediate& RHICmdList)
{
	check(IsInRenderingThread());

	TShaderMapRef<FBoidsRPUpdateLegacyExampleCS> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	FRHIComputeShader* ShaderRHI = ComputeShader.GetComputeShader();
	SetComputePipelineState(RHICmdList, ShaderRHI);

	FRHIBatchedShaderParameters& BatchedParameters = RHICmdList.GetScratchShaderParameters();

	ComputeShader->SetUniformParameters(BatchedParameters, BoidConstantParameters, BoidVariableParameters, LastDeltaTime);
	ComputeShader->SetBufferParameters(BatchedParameters, readRef, writeRef);

	RHICmdList.SetBatchedShaderParameters(ShaderRHI, BatchedParameters);

	FIntVector GroupCounts = FIntVector(FMath::DivideAndRoundUp(BoidConstantParameters.numBoids, BoidsExample_ThreadsPerGroup), 1, 1);
	DispatchComputeShader(RHICmdList, ComputeShader, GroupCounts.X, GroupCounts.Y, GroupCounts.Z);

	FRHIBatchedShaderUnbinds& BatchedUnbinds = RHICmdList.GetScratchShaderUnbinds();
	ComputeShader->UnsetBufferParameters(BatchedUnbinds);
	RHICmdList.SetBatchedShaderUnbinds(ShaderRHI, BatchedUnbinds);

	RHICmdList.CopyBufferRegion(readBuffer, 0, writeBuffer, 0, BoidItemSize * BoidsArray.Num());

	auto readSRVCreateDesc = FRHIViewDesc::CreateBufferSRV()
		.SetType(FRHIViewDesc::EBufferType::Structured)
		.SetNumElements(BoidConstantParameters.numBoids)
		.SetStride(BoidItemSize);
	readRef = RHICmdList.CreateShaderResourceView(readBuffer, readSRVCreateDesc);
}

bool AComputeRPLegacyEmitter::SetNiagaraConstantParameters()
{
	if (Niagara == nullptr || Niagara->GetAsset() == nullptr)
	{
		return false;
	}

	// Get the parameter store of the Niagara component
	FNiagaraUserRedirectionParameterStore& ParameterStore = Niagara->GetOverrideParameters();
	FNiagaraVariable SBParameter = FNiagaraVariable(FNiagaraTypeDefinition(UNiagaraDataInterfaceStructuredBufferLegacy::StaticClass()), TEXT("boidsIn"));
	UNiagaraDataInterfaceStructuredBufferLegacy* data = (UNiagaraDataInterfaceStructuredBufferLegacy*)ParameterStore.GetDataInterface(SBParameter);

	if (data)
	{
		data->numBoids = BoidConstantParameters.numBoids;
		data->SetBuffer(nullptr);
	}

	Niagara->SetIntParameter("numBoids", BoidConstantParameters.numBoids);

	return true;
}

void AComputeRPLegacyEmitter::SetNiagaraVariableParameters()
{
	if (Niagara == nullptr || Niagara->GetAsset() == nullptr)
	{
		return;
	}

	// Get the parameter store of the Niagara component
	FNiagaraUserRedirectionParameterStore& ParameterStore = Niagara->GetOverrideParameters();
	FNiagaraVariable SBParameter = FNiagaraVariable(FNiagaraTypeDefinition(UNiagaraDataInterfaceStructuredBufferLegacy::StaticClass()), TEXT("boidsIn"));
	UNiagaraDataInterfaceStructuredBufferLegacy* data = (UNiagaraDataInterfaceStructuredBufferLegacy*)ParameterStore.GetDataInterface(SBParameter);

	if (data)
	{
		data->numBoids = BoidConstantParameters.numBoids;

		if (!data->GetBuffer().IsValid()
			&& readRef.IsValid())
		{
			data->SetBuffer(readRef);
		}
	}

	Niagara->SetFloatParameter("meshScale", BoidVariableParameters.meshScale);
	Niagara->SetFloatParameter("worldScale", BoidVariableParameters.worldScale);
}