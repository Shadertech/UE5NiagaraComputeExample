#include "ComputeRPDrawer.h"
#include "GraphBuilder/BoidsGB.h"
#include "PixelShaders/BoidsDrawerPS.h"
#include "Settings/ComputeExampleSettings.h"
#include "NiagaraFunctionLibrary.h"
#include "GraphBuilder/BoidsDrawerGB.h"

DEFINE_LOG_CATEGORY(LogComputeRPDrawer);

// ____________________________________________ Constructor

AComputeRPDrawer::AComputeRPDrawer()
{
	Niagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara"));
	Niagara->SetupAttachment(RootComponent);
	Niagara->bAutoActivate = false;

	Plane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Plane"));
	Plane->SetupAttachment(RootComponent);
}

void AComputeRPDrawer::BeginPlay()
{
	Super::BeginPlay();
}

void AComputeRPDrawer::BeginDestroy()
{
	if (Niagara != nullptr )
	{
		Niagara->DeactivateImmediate();
	}

	Super::BeginDestroy();

	if (RenderTarget)
	{
		RenderTarget->ConditionalBeginDestroy(); // Destroy the render target texture
		RenderTarget = nullptr; // Reset the pointer
	}
}

void AComputeRPDrawer::InitComputeShader_GameThread()
{
	check(IsInGameThread());

	CreateRenderTexture();

	const UComputeExampleSettings* computeExampleSettings = UComputeExampleSettings::GetComputeExampleSettings();
	UMaterialInterface* MatInterface = computeExampleSettings->BoidsDrawMatInterface.LoadSynchronous();
	if (MatInterface != nullptr && RenderTarget != nullptr)
	{
		UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(MatInterface, nullptr);
		Plane->SetMaterial(0, MID);
		MID->SetTextureParameterValue(FName("BoidsPos"), RenderTarget);
	}

	Super::InitComputeShader_GameThread();

	BoundsMatrix = FMatrix::Identity.ConcatTranslation(GetActorLocation());
	BoidsArray.Empty(); // Clear any existing elements in the array

	BoidsPingPongBuffer = FPingPongBuffer();
	BoidsRenderGraphPasses = FBoidsRenderGraphPasses();

	Niagara->SetAsset(computeExampleSettings->BoidsDrawerVFX.LoadSynchronous());

	if (SetConstantParameters() && SetDynamicParameters())
	{
		Niagara->Activate(true);
	}
}

void AComputeRPDrawer::InitComputeShader_RenderThread(FRHICommandListImmediate& RHICmdList)
{
	check(IsInRenderingThread());

	Super::InitComputeShader_RenderThread(RHICmdList);

	FRDGBuilder GraphBuilder(RHICmdList);

	BoidsRenderGraphPasses.ClearPasses();

	FGraphBullder_Boids::InitBoidsExample_RenderThread(GraphBuilder, *GetOwnerName(), BoidsArray, BoidCurrentParameters, BoidsRenderGraphPasses, BoidsPingPongBuffer);
	FComputeShader_BoidsDrawer::InitBoidsDrawerExample_RenderThread(GraphBuilder, BoidsArray, BoidsPingPongBuffer);
	FPixelShader_BoidsDrawer::InitDrawToRenderTargetExample_RenderThread(GraphBuilder, BoidsArray, RenderTarget, RenderTargetTexture);

	GraphBuilder.Execute();
}

// ____________________________________________ Execute Compute Shader

void AComputeRPDrawer::ExecuteComputeShader_GameThread(float DeltaTime)
{
	check(IsInGameThread());

	Super::ExecuteComputeShader_GameThread(DeltaTime);

	SetDynamicParameters();
}

void AComputeRPDrawer::ExecuteComputeShader_RenderThread(FRHICommandListImmediate& RHICmdList)
{
	check(IsInRenderingThread());

	Super::ExecuteComputeShader_RenderThread(RHICmdList);

	FRDGBuilder GraphBuilder(RHICmdList);

	BoidsRenderGraphPasses.ClearPasses();

	// The graph will help us figure out when the GPU memory is needed, and only have it allocated from then, so this makes memory management a lot easier and nicer!
	//FRDGTextureDesc ComputeShaderOutputDesc = FRDGTextureDesc::Create2D(FIntPoint(RenderTarget->SizeX, RenderTarget->SizeY), EPixelFormat::PF_R32_UINT, FClearValueBinding::None, ETextureCreateFlags::RenderTargetable | ETextureCreateFlags::UAV | ETextureCreateFlags::ShaderResource);
	FRDGTextureDesc ComputeShaderOutputDesc = FRDGTextureDesc::Create2D(FIntPoint(RenderTarget->SizeX, RenderTarget->SizeY), EPixelFormat::PF_A32B32G32R32F, FClearValueBinding::None, ETextureCreateFlags::RenderTargetable | ETextureCreateFlags::UAV | ETextureCreateFlags::ShaderResource);
	FRDGTextureRef OutputTexture = GraphBuilder.CreateTexture(ComputeShaderOutputDesc, TEXT("ShaderPlugin_ComputeShaderOutput"), ERDGTextureFlags::None);
	FGraphBullder_Boids::ExecuteBoidsExample_RenderThread(GraphBuilder, *GetOwnerName(), BoidCurrentParameters, BoidsRenderGraphPasses, BoidsPingPongBuffer, LastDeltaTime);
	FComputeShader_BoidsDrawer::ExecuteBoidsDrawerExample_RenderThread(GraphBuilder, BoidCurrentParameters, BoidsPingPongBuffer, GraphBuilder.CreateUAV(OutputTexture), RenderTarget);
	FPixelShader_BoidsDrawer::DrawToRenderTargetExample_RenderThread(GraphBuilder, GraphBuilder.CreateSRV(OutputTexture), RenderTarget, RenderTargetTexture);

	GraphBuilder.Execute();
}

void AComputeRPDrawer::DisposeComputeShader_GameThread()
{
	check(IsInGameThread());

	Super::DisposeComputeShader_GameThread();

	RenderTargetTexture = nullptr;
}

void AComputeRPDrawer::DisposeComputeShader_RenderThread(FRHICommandListImmediate& RHICmdList)
{
	check(IsInRenderingThread());

	Super::DisposeComputeShader_RenderThread(RHICmdList);

	BoidsPingPongBuffer.Dispose();
}

bool AComputeRPDrawer::SetConstantParameters()
{
	BoidCurrentParameters.ConstantParameters = BoidConstantParameters;

	// Resize the array to accommodate numboids elements
	BoidsArray.SetNum(BoidCurrentParameters.ConstantParameters.numBoids);

	if (Niagara == nullptr || Niagara->GetAsset() == nullptr)
	{
		return false;
	}

	UNiagaraFunctionLibrary::SetTextureObject(Niagara, "boidPositions", RenderTarget);

	//const UComputeExampleSettings* computeExampleSettings = UComputeExampleSettings::GetComputeExampleSettings();
	Niagara->SetIntParameter("numBoids", BoidCurrentParameters.ConstantParameters.numBoids);

	return true;
}

bool AComputeRPDrawer::SetDynamicParameters()
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

	Niagara->SetFloatParameter("meshScale", BoidCurrentParameters.DynamicParameters.meshScale);
	Niagara->SetFloatParameter("worldScale", BoidCurrentParameters.worldScale);

	return true;
}

void AComputeRPDrawer::CreateRenderTexture()
{
	if (RenderTarget != nullptr)
	{
		return;
	}

	FName TextureName = FName(TEXT("RT_BoidPositions"));
	int32 SizeX = 32; // Width
	int32 SizeY = 32; // Height

	RenderTarget = NewObject<UTextureRenderTarget2D>(this, TextureName);
	RenderTarget->ClearColor = FLinearColor::Black;
	RenderTarget->TargetGamma = 0.0f;
	RenderTarget->LODGroup = TextureGroup::TEXTUREGROUP_Pixels2D;
	RenderTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA32f;
	RenderTarget->AddressX = TextureAddress::TA_Wrap;
	RenderTarget->AddressY = TextureAddress::TA_Wrap;
	RenderTarget->InitAutoFormat(SizeX, SizeY);
	RenderTarget->UpdateResourceImmediate(true);
}