#include "ComputeRPLegacyEmitter.h"
#include "Kismet/GameplayStatics.h"
#include "SceneRendering.h"
#include "ScenePrivate.h"
#include "ComputeShaders/BoidsRPLegacyCS.h"
#include "Settings/ComputeExampleSettings.h"
#include "DataDrivenShaderPlatformInfo.h"
#include "Niagara/NDIStructuredBufferLegacyFunctionLibrary.h"

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
}

// ____________________________________________ BeginDestroy

void AComputeRPLegacyEmitter::BeginDestroy()
{
	if (Niagara != nullptr)
	{
		Niagara->DeactivateImmediate();
	}
	Super::BeginDestroy();
}

// ____________________________________________ DisposeComputeShader
void AComputeRPLegacyEmitter::DisposeComputeShader_GameThread()
{
	Super::DisposeComputeShader_GameThread();
}

void AComputeRPLegacyEmitter::DisposeComputeShader_RenderThread(FRHICommandListImmediate& RHICmdList)
{
	check(IsInRenderingThread());

	Super::DisposeComputeShader_RenderThread(RHICmdList);

	if (readBuffer->IsValid())
	{
		readBuffer = nullptr;
	}
	if (writeBuffer->IsValid())
	{
		writeBuffer = nullptr;
	}
	if (readRef->IsValid())
	{
		readRef = nullptr;
	}
	if (writeRef->IsValid())
	{
		writeRef = nullptr;
	}
}

// ____________________________________________ Init Compute Shader

void AComputeRPLegacyEmitter::InitComputeShader_GameThread()
{
	check(IsInGameThread());

	Super::InitComputeShader_GameThread();

	BoundsMatrix = FMatrix::Identity.ConcatTranslation(GetActorLocation());
	BoidsArray.Empty(); // Clear any existing elements in the array

	const UComputeExampleSettings* computeExampleSettings = UComputeExampleSettings::GetComputeExampleSettings();
	Niagara->SetAsset(computeExampleSettings->BoidsEmitterLegacyVFX.LoadSynchronous());

	if (SetConstantParameters() && SetDynamicParameters())
	{
		Niagara->Activate(true);
	}
}

void AComputeRPLegacyEmitter::InitComputeShader_RenderThread(FRHICommandListImmediate& RHICmdList)
{
	check(IsInRenderingThread());

	Super::InitComputeShader_RenderThread(RHICmdList);

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
	writeBuffer = RHICmdList.CreateStructuredBuffer(BoidItemSize, BoidItemSize * BoidsArray.Num(), BUF_UnorderedAccess | BUF_ShaderResource, CreateWriteInfo);
	auto readSRVCreateDesc = FRHIViewDesc::CreateBufferSRV()
		.SetType(FRHIViewDesc::EBufferType::Structured)
		.SetNumElements(BoidCurrentParameters.ConstantParameters.numBoids)
		.SetStride(BoidItemSize);

	readRef = RHICmdList.CreateShaderResourceView(readBuffer, readSRVCreateDesc);
	writeRef = RHICmdList.CreateUnorderedAccessView(writeBuffer, false, false);
	FRHIBatchedShaderParameters& BatchedParameters = RHICmdList.GetScratchShaderParameters();

	ComputeShader->SetUniformParameters(BatchedParameters, BoidCurrentParameters, 0.0f);
	ComputeShader->SetBufferParameters(BatchedParameters, nullptr, writeRef);

	RHICmdList.SetBatchedShaderParameters(ShaderRHI, BatchedParameters);

	FIntVector GroupCounts = FIntVector(FMath::DivideAndRoundUp(BoidCurrentParameters.ConstantParameters.numBoids, BoidsExample_ThreadsPerGroup), 1, 1);
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

void AComputeRPLegacyEmitter::ExecuteComputeShader_GameThread(float DeltaTime)
{
	check(IsInGameThread());

	Super::ExecuteComputeShader_GameThread(DeltaTime);

	SetDynamicParameters();
}

void AComputeRPLegacyEmitter::ExecuteComputeShader_RenderThread(FRHICommandListImmediate& RHICmdList)
{
	check(IsInRenderingThread());

	Super::ExecuteComputeShader_RenderThread(RHICmdList);

	TShaderMapRef<FBoidsRPUpdateLegacyExampleCS> ComputeShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	FRHIComputeShader* ShaderRHI = ComputeShader.GetComputeShader();
	SetComputePipelineState(RHICmdList, ShaderRHI);

	FRHIBatchedShaderParameters& BatchedParameters = RHICmdList.GetScratchShaderParameters();

	ComputeShader->SetUniformParameters(BatchedParameters, BoidCurrentParameters, LastDeltaTime);
	ComputeShader->SetBufferParameters(BatchedParameters, readRef, writeRef);

	RHICmdList.SetBatchedShaderParameters(ShaderRHI, BatchedParameters);
	FIntVector GroupCounts = FIntVector(FMath::DivideAndRoundUp(BoidCurrentParameters.ConstantParameters.numBoids, BoidsExample_ThreadsPerGroup), 1, 1);
	DispatchComputeShader(RHICmdList, ComputeShader, GroupCounts.X, GroupCounts.Y, GroupCounts.Z);
	FRHIBatchedShaderUnbinds& BatchedUnbinds = RHICmdList.GetScratchShaderUnbinds();
	ComputeShader->UnsetBufferParameters(BatchedUnbinds);
	RHICmdList.SetBatchedShaderUnbinds(ShaderRHI, BatchedUnbinds);

	RHICmdList.CopyBufferRegion(readBuffer, 0, writeBuffer, 0, BoidItemSize * BoidsArray.Num());
	auto readSRVCreateDesc = FRHIViewDesc::CreateBufferSRV()
		.SetType(FRHIViewDesc::EBufferType::Structured)
		.SetNumElements(BoidCurrentParameters.ConstantParameters.numBoids)
		.SetStride(BoidItemSize);
	readRef = RHICmdList.CreateShaderResourceView(readBuffer, readSRVCreateDesc);
}

bool AComputeRPLegacyEmitter::SetConstantParameters()
{
	const UComputeExampleSettings* computeExampleSettings = UComputeExampleSettings::GetComputeExampleSettings();
	BoidCurrentParameters.ConstantParameters = computeExampleSettings->BoidConstantParameters;

	// Resize the array to accommodate numboids elements
	BoidsArray.SetNum(BoidCurrentParameters.ConstantParameters.numBoids);

	if (Niagara == nullptr || Niagara->GetAsset() == nullptr)
	{
		return false;
	}

	UNDIStructuredBufferLegacyFunctionLibrary::SetNiagaraStructuredBuffer(Niagara, "boidsIn", BoidCurrentParameters.ConstantParameters.numBoids, nullptr);

	Niagara->SetIntParameter("numBoids", BoidCurrentParameters.ConstantParameters.numBoids);

	return true;
}

bool AComputeRPLegacyEmitter::SetDynamicParameters()
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

	UNDIStructuredBufferLegacyFunctionLibrary::SetNiagaraStructuredBuffer(Niagara, "boidsIn", BoidCurrentParameters.ConstantParameters.numBoids, readRef);

	Niagara->SetFloatParameter("meshScale", BoidCurrentParameters.DynamicParameters.meshScale);
	Niagara->SetFloatParameter("worldScale", BoidCurrentParameters.worldScale);

	return true;
}